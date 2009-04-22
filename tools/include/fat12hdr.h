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
 
/* Floppy header of FAT12 */
BS_OEMName:	.ascii  "KEN CCAO"	/* OEM String, 8 bytes required */
BPB_BytsPerSec:	.2byte  512		/* Bytes per sector */
BPB_SecPerClus:	.byte   1		/* Sector per cluster */
BPB_ResvdSecCnt:.2byte  1		/* Reserved sector count */
BPB_NumFATs:	.byte   2		/* Number of FATs */
BPB_RootEntCnt:	.2byte  224		/* Root entries count */
/* * */
BPB_TotSec16:	.2byte  65520		/* Total sector number.
					 * 2880 for floppy;
					 * 65520 for 32 MB kenos harddisk. */
/* * */
BPB_Media:	.byte   0xf8		/* Media descriptor. 
					 * 0xf0 for removable device;
					 * 0xf8 for fixed device. */
BPB_FATSz16:	.2byte  9		/* FAT size(sectors) */
/* * */
BPB_SecPerTrk:	.2byte  63		/* Sector per track.
					 * 18 for 1.44MB floppy;
					 * 63 for 32MB kenos harddisk. */
/* * */
BPB_NumHeads:	.2byte  16		/* Number of magnetic heads
					 * 2 for 1.44MB floppy;
					 * 16 for 32MB kenos hardisk */
BPB_HiddSec:	.4byte  0		/* Number of hidden sectors */
BPB_TotSec32:	.4byte  0		/* If TotSec16 equal 0, this works */
/* * */
BS_DrvNum:	.byte	0x80		/* Driver number of interrupt 13.
					 * 0x0 for floppy;
					 * 0x80 for hardisk. */
BS_Reserved1:	.byte   0		/* Reserved */
BS_BootSig:	.byte   0x29		/* Boot signal */
BS_VolID:	.4byte  0		/* Volume ID */
BS_VolLab:	.ascii  "enthus 0.0x"	/* Volume label, 11 bytes required */
BS_FileSysType:	.ascii  "FAT12   "	/* File system type, 8 bytes required */

.set    RootDirSectors,  14        /* Root directory sector count */
.set    SecNoOfRootDir,  19        /* 1st sector of root directory */
.set    SecNoOfFAT1,     1         /* 1st sector of FAT1 */
.set    DeltaSecNo,      17        /* BPB_(RsvdSecCnt+NumFATs*FATSz) -2 */
/* Start sector of file space =*/
