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

int fat_next_tabent(int tab_ent)
{
	int ibytes = tab_ent * 2; /* 2 bytes/entry */
	int ri = ibytes % BLOCK_SIZE; /* relative ibytes */
	
	/* FAT1 starts at 512bytes - second blk */
	int offset = ibytes / BLOCK_SIZE + 1;
	
	int lo = 0, hi = 0, next_ent = 0;
	char buf[BLOCK_SIZE];
	
	unsigned int t = 0;
	
	/* read one blk from offset into buf */
	t = ide_rblks(0, offset, 1, buf);
	
	lo = (int)buf[ri] & 0xff;
	hi = (int)buf[ri + 1] & 0xff;
	
	next_ent = hi * 256 + lo;

	return next_ent;
}




