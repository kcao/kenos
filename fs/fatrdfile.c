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

#include "type.h"		/* needed by drv/hd.h */
#include "string.h"		/* memcpy */
#include "drv/hd.h"		/* ide_rw */
#include "fs/fat1x.h"		/* fat constants */

#include "fs/fatroot.h"
#include "fs/fattabe.h"
#include "fs/fatrdclus.h"
#include "fs/fatcmpfname.h"

/* no current index of the file is reading! */
int fat_rdfile(char fcontent[], int nbytes, iFILE f_ent_nr)
{
	int ent_nr = f_ent_nr;
	int offset = 0;
	int nblk = 0, rbytes = 0; /* reminding bytes nr */
	char buf[CLUS_SIZE];
	int i = 0;
	
	nblk = nbytes / CLUS_SIZE;
	
	for (i = 0; i < nblk; i ++) {
		offset = data_clus_offset(ent_nr);
		
		fat_rdclus(&fcontent[i * CLUS_SIZE], offset);
		/* next cluster */
		ent_nr = fat_next_tabent(ent_nr);
	}
	/* get one more clus to fill the half sector */
	rbytes = nbytes % CLUS_SIZE;
	if (rbytes > 0 ) {
		offset = data_clus_offset(ent_nr);
		fat_rdclus(buf, offset);
		
		memcpy(&fcontent[i * CLUS_SIZE], buf, rbytes);
	}
	
	return 0;
}


