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


#endif /* _KENOS_IDE_H_ */

