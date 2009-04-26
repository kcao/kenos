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

#include "type.h"
#include "drv/hd.h"
#include "fs/fat1x.h"
#include "string.h"
#include "utils.h"

/* root directory begin sector */
/* 0x2600 / 0x200 = 0x13 */
/* FAT_ROOT_BEG	--> 19 (in fs/fatconst.h) */

/* 0 is the first one, 
 * the 32bytes result stored in rebuf[32], root entry buffer. */
int fat_root_ent(int ent_n, char rebuf[])
{
	
	int roffset = ent_n / 16; /* 16 entries/sector */
	int offset = FAT_ROOT_BEG + roffset;
	/* entry begin at ent_beg in buf[] */
	int ent_beg = (ent_n % 16) * 32; /* 32 bytes/entry */
	
	char buf[CLUS_SIZE];
	
//	kinfo("in fat_root_ent...0");
	unsigned int t = 0;
	/* read one blk from offset into buf */
	t = ide_rblks(0, offset, 1, buf);
	
	memcpy(rebuf, &buf[ent_beg], 32);
	
	return 0;
}

/* 
 * (fopen in c??)
 * read root dir, to get the root entry of the file.
 * 
 * store the result in rent[32].
 * return the first cluster nr of the file,
 * 	<= 0 --> no such file.
 */
iFILE get_root_ent_bname(char *fname, char rent[])
{
	int i = 0;
	char buf[32];
	/* low byte and high byte of first cluster number */
	int lo = 0, hi = 0;
	iFILE firstclus = 0;

	for(i = 0; i < FAT_ROOT_ENT_CNT; i++) {
		
		/* get one more entry */
		fat_root_ent(i, buf);
//		disp_int(buf[0]);

		if (0 == cmpfname(fname, buf)) {
			kinfo("found file");
			break;
		}
	}
	if (i == FAT_ROOT_ENT_CNT) {
		/* cannot find the file in root dir */
		kinfo("cannot find file");
		return -1;
	}

	/* find the entry for the file. get first cluster from the buf. */
	lo = (int)buf[0x1a] & 0xff;
	hi = (int)buf[0x1b] & 0xff;
	firstclus = hi * 256 + lo;
	
	memcpy(rent, buf, 32);
	return firstclus;
}

/* calculate the size of file, by bytes number. 
 * 
 * return the number of bytes. 
 */
long fat_fsize(char rent[])
{
	int b1 = 0, b2 = 0, b3 = 0, b4 = 0;
	long fsize = 0;
	
	b1 = (int)rent[0x1c] & 0xff;
	b2 = (int)rent[0x1d] & 0xff;
	b3 = (int)rent[0x1e] & 0xff;
	b4 = (int)rent[0x1f] & 0xff;
	
	fsize = b4 * 0x1000000 + b3 * 0x10000 + b2 * 0x100 + b1;
	
	return fsize;
}



