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
#include "drv/ide.h"
#include "asm/io.h"
#include "ksync.h"

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
		perror("not ready to sel\n");
		return -1;
	}

	/* Select the drive. The spec says to wait at least 400 ns before
	   reading the status register to ensure its content is valid. */
	outb(0xa0 | (dev->position << 4), iobase + ATA_DRV_HEAD);
	delay(1);

	/* By now, we should have BSY = 0 and DRQ = 0.
	   See Device Selection Protocol, ATA/ATAPI-4 spec, section 9.6. */
	if ((inb(iobase + ATA_STATUS) & (ATA_STATUS_BSY | ATA_STATUS_DRQ))) {
		perror("failed to sel\n");
		return -2;
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
//	disp_str("id_dev\n");
	
}

/*
 * Returns the IDE device associated with the specified minor number.
 */
static struct ide_device *get_ide_dev(unsigned int minor)
{
	struct ide_controller *ctrl;
	struct ide_device *idev;

	ASSERT(minor < NR_IDE_CONTROLLERS * NR_DEVICES_PER_CONTROLLER);

	ctrl = &controllers[minor / NR_DEVICES_PER_CONTROLLER];
	idev = &ctrl->devices[minor % NR_DEVICES_PER_CONTROLLER];
	return idev;
}

/**/
static unsigned int ide_rw_blks(unsigned int minor,
					t_32 block,
					unsigned int nblocks,
					void *buffer, 
					int type)
{
	struct ide_device *idev;
	struct ide_controller *ictrl;
	t_8 sc, cl, ch, hd, cmd;
	int iobase, i;
	t_16 *buf = (t_16 *) buffer;
	
	idev = get_ide_dev(minor);
	if (!idev->present)
		return 0;

	if (!nblocks) {
		return 0;
	}

	if (nblocks > MAX_NBLOCKS) {
		nblocks = MAX_NBLOCKS;
	}

	if (block + nblocks > idev->capacity) {
		perror("out of dev capacity.");
		return -1;
	}

	ictrl = idev->controller;
	iobase = ictrl->iobase;
	
	if (0 > kmutex_lock(&(ictrl->mutex))) {
		perror("cannot get resource.");
		return -1;
	}
	
	if (0 != select_dev(idev)) {
		kmutex_unlock(&(ictrl->mutex));
		perror("failed to select dev");
		return -1;
	}
	
	if (idev->lba) {
		sc = block & 0xff;
		cl = (block >> 8) & 0xff;
		ch = (block >> 16) & 0xff;
		hd = (block >> 24) & 0xf;
	} else {
		/* See http://en.wikipedia.org/wiki/CHS_conversion */
		int cyl = block / (idev->heads * idev->sectors);
		int tmp = block % (idev->heads * idev->sectors);
		sc = tmp % idev->sectors + 1;
		cl = cyl & 0xff;
		ch = (cyl >> 8) & 0xff;
		hd = tmp / idev->sectors;
	}
	
	cmd = (type == IO_READ) ? ATA_READ_BLOCK : ATA_WRITE_BLOCK;

	/* See ATA/ATAPI-4 spec, section 8.27.4 */
	outb(nblocks, iobase + ATA_NSECTOR);
	outb(sc, iobase + ATA_SECTOR);
	outb(cl, iobase + ATA_LCYL);
	outb(ch, iobase + ATA_HCYL);
	outb((idev->lba << 6) | (idev->position << 4) | hd, 
		iobase + ATA_DRV_HEAD);
	outb(cmd, iobase + ATA_COMMAND);

	/* The host shall wait at least 400 ns before reading the 
	 * Status register.
	 * See PIO data in/out protocol in ATA/ATAPI-4 spec. */
	delay(1);
	
	/* Wait at most 30 seconds for the BSY flag to be cleared. */
	if (0 == wait4ctrl
	    		(ictrl, ATA_STATUS_BSY, 0, ATA_TIMEOUT)) {
		kmutex_unlock(&(ictrl->mutex));
		perror("dev seems to busy");
		return -1;
	}
	
	/* Did the device report an error? */
	if (inb(iobase + ATA_STATUS) & ATA_STATUS_ERR) {
		kmutex_unlock(&(ictrl->mutex));
		perror("met error before writing");
		return 0;
	}

	if (type == IO_WRITE) {
		/* Transfer the data to the controller. */
	//	for (i = 0; i < nblocks * 256; i++, buf++)
		for (i = nblocks * 256; i > 0; i--, buf++)
			outw(*buf, iobase + ATA_DATA);
	}
	
	/* Go to sleep until the IRQ handler wakes us up.
	 * Note: on Bochs, the IRQ is raised before we even reach this line!
	 * This is OK, and in that case, this line will not make us go to
	 * sleep (the semaphore will have been incremented by the IRQ handler
	 * prior to reaching this line) */
/**/	while (0 > ksema_p(&(ictrl->io_sema))) {
		kinfo("wait for resource.");
		delay(10);
		
	}
	
	/* Did the device report an error? */
	if (inb(iobase + ATA_STATUS) & ATA_STATUS_ERR) {
		kmutex_unlock(&(ictrl->mutex));
		perror("met error before reading");
		return -1;
	}

	if (type == IO_READ) {
		/* Copy the data to the destination buffer. */
	//	for (i = nblocks * 256; --i >= 0;)
	//		*buf++ = inw(iobase + ATA_DATA);
		kinfo("I read...");
		for (i = nblocks * 256; i > 0; i--, buf++) {
			*buf = inw(iobase + ATA_DATA);
		}
	}

	kmutex_unlock(&(ictrl->mutex));
	kinfo("IO good");
	return nblocks;
}

/*
 * Read the specified block from the specified device, 
 * and copy its content to the destination buffer. 
 * The work is delegated to ide_rw_blocks.
 */
unsigned int ide_rblks(unsigned int minor, t_32 block,
				unsigned int nblocks, void *buffer)
{
	return ide_rw_blks(minor, block, nblocks, buffer, IO_READ);
}

/*
 * Write the content of the source buffer in the specified block 
 * of thespecified device. 
 * The work is delegated to ide_rw_blks.
 */
unsigned int ide_wblks(unsigned int minor, t_32 block,
				unsigned int nblocks, void *buffer)
{
	return ide_rw_blks(minor, block, nblocks, buffer, IO_WRITE);
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

static void ide_handler(struct ide_controller *ictrl)
{
    /* This wakes up the task waiting for the I/O operation to complete. */
    ksema_v(&(ictrl->io_sema));
}

static void pri_ide_handler(int irq)
{
	struct ide_controller *ictrl = &controllers[PRIMARY_IDE_CONTROLLER];
	ide_handler(ictrl);
}

static void sec_ide_handler(int irq)
{
	struct ide_controller *ictrl = &controllers[SECONDARY_IDE_CONTROLLER];
	ide_handler(ictrl);
}

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
		
		kmutex_init(&(i_ctrl->mutex));
		ksema_init(&(i_ctrl->io_sema), 0);
		
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

