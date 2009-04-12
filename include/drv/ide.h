/***************************************************************************
 *   Copyright (C) 2009 by Cao, Chen                                       *
 *   ken.ccao@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef	_KENOS_IDE_H_
#define	_KENOS_IDE_H_

/* We support up to 2 IDE controllers in Simplix. */
#define NR_IDE_CONTROLLERS 2

/* Per ATA spec... */
#define NR_DEVICES_PER_CONTROLLER 2

/* Position of each IDE controller. */
#define PRIMARY_IDE_CONTROLLER    0
#define SECONDARY_IDE_CONTROLLER  1

/* Base I/O ports of first and second ATA/IDE controllers.
   These are standard values for a PC. */
#define PRIMARY_IDE_CONTROLLER_IOBASE   0x1F0
#define SECONDARY_IDE_CONTROLLER_IOBASE 0x170

/* Registers offsets. */
#define ATA_DATA        0
#define ATA_ERROR       1
#define ATA_NSECTOR     2	/* sector count */
#define ATA_SECTOR      3	/* sector number */
#define ATA_LCYL        4
#define ATA_HCYL        5
#define ATA_DRV_HEAD    6
#define ATA_STATUS      7
#define ATA_COMMAND     7
#define ATA_DEV_CTL     0x206

/* ATA protocol commands. */
#define ATA_IDENTIFY    0xEC
#define ATAPI_IDENTIFY  0xA1
#define ATA_READ_BLOCK  0x20
#define ATA_WRITE_BLOCK 0x30

/* Important bits in the status register of an ATA controller.
   See ATA/ATAPI-4 spec, section 7.15.6 */
#define ATA_STATUS_BSY  0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ  0x08
#define ATA_STATUS_ERR  0x01

/* Important bits in the device control register.
   See ATA/ATAPI-4 spec, section 7.9.6 */
#define ATA_CTL_SRST    0x04
#define ATA_CTL_nIEN    0x02

/* Maximum timeout, in microseconds, for all commands (= 30 seconds) */
#define ATA_TIMEOUT 30000000

/* Operation types. */
#define IO_READ     0
#define IO_WRITE    1

/* Device position in the ATA chain. */
#define MASTER      0
#define SLAVE       1

/* The block size, in bytes. */
#define BLOCK_SIZE  512

/* The max. number of blocks this IDE driver can read/write in one operation. */
#define MAX_NBLOCKS 256


#include "ksync.h"

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
	struct_kmutex mutex;

	/* When issuing a request to the IDE controller, a task decrements
	   the value of this semaphore (DOWN). The IRQ handler increments it
	   when the I/O operation has completed. */
	struct ksema io_sema;
};


#endif /* _KENOS_IDE_H_ */

