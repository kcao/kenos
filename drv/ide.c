/*===========================================================================
 *
 * ide.c
 *
 * Copyright (C) 2007 - Julien Lecomte
 *
 * Modified by Cao, Chen <ken.ccao@gmail.com> - 2009
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *===========================================================================
 *
 * IDE Hard Disk driver.
 *
 * A great (although non trivial) improvement would be to optimize drive head
 * motion by writing an I/O scheduler implementing a basic elevator algorithm.
 * This means we would need to introduce the notion of I/O request.
 * When choosing which request should execute next, we need to strike
 * a balance between the age of the pending requests, their initial priority
 * (if we support request priorities), and the time it would take for the
 * appropriate drive head to move to the right track to handle a request.
 * Interesting stuff but definitely non trivial! (also I wonder if a parameter
 * of this algorithm would be the actual measured speed of the drive head...)
 *
 *===========================================================================*/

#include "utils.h"
#include "ide.h"
#include "asm/io.h"

struct ide_device {

	/* Pointer to the controller managing this device. */
	struct ide_controller *controller;

	/* Position of this device in the ATA chain (MASTER/SLAVE) */
	t_8 position:1;

	/* Indicates whether this device was successfully identified.
	   If this bit is not set, the information included in this
	   structure below this point is not valid. */
	t_8 present:1;

	/* Does this device support the PACKET command feature set? */
	t_8 atapi:1;

	/* Does this device support LBA addressing? */
	t_8 lba:1;

	/* Is DMA supported by this device? */
	t_8 dma:1;

	/* General information about the device. */
	char model[40];
	char serial[20];
	char firmware[8];

	/* Disk geometry. */
	unsigned int cylinders;
	unsigned int heads;
	unsigned int sectors;
	unsigned int capacity;
};

struct ide_controller {

	/* Base I/O port:
	   0x1F0 for the 1st controller
	   0x170 for the 2nd controller */
	int iobase;

	/* List of devices attached to this controller. */
	struct ide_device devices[NR_DEVICES_PER_CONTROLLER];

	/* A controller can serve only one request at a time. This mutex
	   protects the controller while it's being used by another task. */
//	struct kmutex *mutex;

	/* When issuing a request to the IDE controller, a task decrements
	   the value of this semaphore (DOWN). The IRQ handler increments it
	   when the I/O operation has completed. */
//	struct ksema *io_sema;
};

/* Kenos supports up to 2 IDE controllers addressable via their standard
   I/O ports. A PC may have more than 2 IDE controllers and controllers may
   use different I/O ports. We don't handle these cases. */
static struct ide_controller controllers[NR_IDE_CONTROLLERS] = {
	{
	 .iobase = PRIMARY_IDE_CONTROLLER_IOBASE,
	 .devices = {
		     {.position = MASTER},
		     {.position = SLAVE}
		     }
	 },
	{
	 .iobase = SECONDARY_IDE_CONTROLLER_IOBASE,
	 .devices = {
		     {.position = MASTER},
		     {.position = SLAVE}
		     }
	 }
};

/*
 * The characters in the strings returned by the IDENTIFY command are
 * swapped (the spec mentions it) E.g 'eGenir c2143' => 'Generic 2143'
 * This function unscrambles them and wipes out any trailing garbage.
 */
static void fix_ide_string(char *s, int len)
{
	char c, *p = s, *end = s + (len & ~1);

	/* Swap characters. */
	while (p != end) {
		c = *p;
		*p = *(p + 1);
		*(p + 1) = c;
		p += 2;
	}

	/* Make sure we have a NULL byte at the end. Wipe out trailing garbage. */
	p = end - 1;
	*p-- = '\0';
	while (p-- != s) {
		c = *p;
		if (c > 32 && c < 127)
			break;
		*p = '\0';
	}
}


/*
 * Wait for the bits specified by the bitmask in the specified controller
 * status register to have the specified value, or for the specified timeout
 * (in microseconds) to expire. Returns whether or not the timeout expired.
 */
static t_8 wait4ctrl(struct ide_controller *controller,
				  t_8 mask, t_8 value,
				  unsigned long timeout)
{
	t_8 status;
	do {
		status = inb(controller->iobase + ATA_STATUS);
		delay(1);	/* Wait a little before trying again. */
	} while ((status & mask) != value && --timeout);
	
	return timeout;
}

/*
 * Resets the specified controller. 
 *
 * as unix/linux convention: return 0 as success, others failed. 
 *
 * See ATA/ATAPI-4 spec, section 9.3.
 */
static int reset_ctrl(struct ide_controller *controller)
{
	int iobase = controller->iobase;

	/* Set the SRST bit in the control register. The spec says that the host
	   shall not begin polling the status register until at least 2 ms after
	   the SRST bit has been set. */
	outb(ATA_CTL_SRST, iobase + ATA_DEV_CTL);
	delay(2);

	/* The device is supposed to set the BSY flag within 400 ns of detecting
	   that the SRST bit has been set. */
	if (0 == wait4ctrl(controller, ATA_STATUS_BSY, ATA_STATUS_BSY, 1)) {
		return -1;
	}

	/* The spec says that the device shall wait until the host resets the SRST
	   before proceeding any further with the reset sequence. */
	outb(0, iobase + ATA_DEV_CTL);

	/* Wait at most 30 seconds for the BSY flag to be cleared. */
	if (0 == wait4ctrl(controller, ATA_STATUS_BSY, 0, ATA_TIMEOUT)) {
		return -1;
	}

	return 0;
}

/* 
 * as unix/linux convention: return 0 as success, others failed. 
 * See ATA/ATAPI-4 spec, section 9.6.
 */
static int select_dev(struct ide_device *dev)
{
	int iobase = dev->controller->iobase;

	/* At this point, we must ensure that BSY = 0 and DRQ = 0.
	   See Device Selection Protocol, ATA/ATAPI-4 spec, section 9.6 */
	if ((inb(iobase + ATA_STATUS) & (ATA_STATUS_BSY | ATA_STATUS_DRQ))) {
		disp_str("no ready to sel\n");
		return -1;
	}

	/* Select the drive. The spec says to wait at least 400 ns before
	   reading the status register to ensure its content is valid. */
	outb(0xa0 | (dev->position << 4), iobase + ATA_DRV_HEAD);
	delay(1);

	/* By now, we should have BSY = 0 and DRQ = 0.
	   See Device Selection Protocol, ATA/ATAPI-4 spec, section 9.6. */
	if ((inb(iobase + ATA_STATUS) & (ATA_STATUS_BSY | ATA_STATUS_DRQ))) {
		disp_str("failed to sel\n");
		return -1;
	}

	return 0;
}


/*
 * Tries to detect and identify the specified IDE device.
 * See http://www.osdev.org/wiki/ATA_PIO_Mode#IDENTIFY_command
 */
static void identify_ide_dev(struct ide_device *i_dev)
{
	int i, iobase = i_dev->controller->iobase;
	t_8 status, cl, ch, cmd;
	t_16 info[256];

	i_dev->present = FALSE;
	
	/* detect the presence of an ATA controller */
	outb(0xab, iobase + ATA_NSECTOR);
	if (inb(iobase + ATA_NSECTOR) != 0xab) {
/*		disp_str("********************\n");
		disp_str("No ATA controller\n");
		disp_str("********************\n");
*/
		return;
	}
	
	/* seems that it is ok that the reset failed */
	if (0 != reset_ctrl(i_dev->controller)) {
/*		disp_str("********************\n");
		disp_str("Failed to RESET dev\n");
		disp_str("********************\n");
*/
		return;
	}
	
	if (0 != select_dev(i_dev)) {
/*		disp_str("********************\n");
		disp_str("Failed to SELECT dev\n");
		disp_str("********************\n");
*/
		return;
	}
	
	/* get the device type
	 *
	 * See ATA/ATAPI-4 spec, section 8.12.5.2 and 9.1.
	 * also refer to bochs' BIOS listing (rombios.c) for more details.
	 * not portable??
	 */
	if (inb(iobase + ATA_NSECTOR) == 0x01 &&
	    inb(iobase + ATA_SECTOR) == 0x01) {
	    
		cl = inb(iobase + ATA_LCYL);
		ch = inb(iobase + ATA_HCYL);
		status = inb(iobase + ATA_STATUS);
		
		if (cl == 0x14 && ch == 0xeb) {
		/* This device implements PACKET Command feature set. */
			i_dev->present = TRUE;
			i_dev->atapi = TRUE;
		} else if (cl == 0 && ch == 0 && status != 0) {
		/* This device does not imple PACKET Command feature set. */
			i_dev->present = TRUE;
			i_dev->atapi = FALSE;
		}
	}
	
	if (FALSE == i_dev->present) {
		disp_str("type: device not exists\n");
		return;
	}
	
/*	if (FALSE == i_dev->atapi) {
		disp_str("No ATAPI\n");
	} else {
		disp_str("ATAPI\n");
	}
*/	
	cmd = i_dev->atapi ? ATAPI_IDENTIFY : ATA_IDENTIFY;

	/* Send the IDENTIFY (PACKET) DEVICE command. */
	outb(cmd, iobase + ATA_COMMAND);
	delay(1);
	
	/* See ATA/ATAPI-4 spec, section 9.7 */
	if (0 == wait4ctrl(i_dev->controller,
			ATA_STATUS_BSY | ATA_STATUS_DRQ | ATA_STATUS_ERR,
			ATA_STATUS_DRQ,
			ATA_TIMEOUT)) {
		i_dev->present = FALSE;
		disp_str("id: device not exists\n");
		return;
	}
	
	/* The IDENTIFY command succeded. Collect the data. */
	for (i = 0; i < 256; i++)
		info[i] = inw(iobase + ATA_DATA);

	i_dev->lba = (info[49] >> 9) & 1;
	i_dev->dma = (info[49] >> 8) & 1;

	i_dev->cylinders = (unsigned int) info[1];
	i_dev->heads = (unsigned int) info[3];
	i_dev->sectors = (unsigned int) info[6];

	/* Here, we simplified things a bit.
	   See ATA/ATAPI-4 spec, Annexe B for more information. */
	if (i_dev->lba) {
		i_dev->capacity = (unsigned int) info[60];
	} else {
		i_dev->capacity =
		    i_dev->heads * i_dev->sectors * i_dev->cylinders;
	}
	
	/* Copy and massage the information that is useful to us. */
	memcpy(i_dev->model, &info[27], 40);
	memcpy(i_dev->serial, &info[10], 20);
	memcpy(i_dev->firmware, &info[23], 8);

	fix_ide_string(i_dev->model, 40);
	fix_ide_string(i_dev->serial, 20);
	fix_ide_string(i_dev->firmware, 8);
/*	
	disp_str(i_dev->model);
	disp_str(i_dev->serial);
	disp_str(i_dev->firmware);
*/	
		
	disp_str("id_dev\n");
	
}

void print_ide_info(struct ide_device *idev, int i)
{
	disp_str((idev->atapi ? "CD-ROM " : "Hard Disk "));
	disp_str("[");
	disp_int(i);
	disp_str("-");
	disp_int(idev->position);
	disp_str("]: ");
	disp_str(idev->model);
	disp_str(" (");
	disp_int(idev->cylinders);
	disp_str("-");
	disp_int(idev->heads);
	disp_str("-");
	disp_int(idev->sectors);
	disp_str(" = ");
	disp_int(idev->capacity);
	disp_str(" sectors)\nLBA:");
	disp_str((idev->lba ? "YES" : "NO"));
	disp_str(" - DMA:");
	disp_str((idev->dma ? "YES" : "NO"));
	disp_str("\n\n");
}

void pri_ide_handler(int irq)
{}

void sec_ide_handler(int irq)
{}

/*
 * Detect IDE devices and register IRQ handlers.
 * This function is called at boot time only!
 */
void init_ide_dev(void)
{
	int i, j;
	struct ide_device *i_dev;
	struct ide_controller *i_ctrl;
	char msg[256];
	
	/* register dev here*/
//	if (register_blkdev_class
//	    (BLKDEV_IDE_DISK_MAJOR, "IDE Hard Disk Driver",
//	     &ide_read_blocks, &ide_write_blocks) != S_OK)
//		return;
	/* registration end */
	
	/**/
	for (i = 0; i < NR_IDE_CONTROLLERS; i++) {
		
		i_ctrl = &controllers[i];	
	//	controller->mutex = kmutex_init();
	//	controller->io_sema = ksema_init(0);
		
		/* Detect and identify IDE devices attached to this
		 * controller. */
		for (j = 0; j < NR_DEVICES_PER_CONTROLLER; j++) {
			i_dev = &i_ctrl->devices[j];
			i_dev->controller = i_ctrl;
		/*	
			disp_str("ctrl = ");
			disp_int(i);
			disp_str("; s/m = ");
			disp_int(j);
			disp_str("\n");
		*/
			identify_ide_dev(i_dev);
			
			if (!i_dev->present || i_dev->atapi) {
				continue;
			}

			/* Show the device information on the screen.
			 * Since we don't support ATAPI devices, 
			 * let's not list them! */
			
			print_ide_info(i_dev, i);


//			/* Register the device with the block device 
//			 * subsystem. */
//			register_blkdev_instance(BLKDEV_IDE_DISK_MAJOR,
//						 i *
//						 NR_DEVICES_PER_CONTROLLER
//						 + j, msg, BLOCK_SIZE,
//						 device->capacity);
		}
	}
	
	put_irq_handler(PRI_IDE_IRQ, pri_ide_handler);
	put_irq_handler(SEC_IDE_IRQ, sec_ide_handler);
	
	enable_irq(PRI_IDE_IRQ);
	enable_irq(SEC_IDE_IRQ);

}

