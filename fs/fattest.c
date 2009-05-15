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

#include "fs/fattabe.h"
#include "fs/fatroot.h"
#include "fs/fatrdfile.h"

void test_ftab()
{
	int res = fat_next_tabent(0x405);
	
	disp_str("\n===========\n");
	disp_int(res);
	disp_str("                 \n===========\n");
	
	
}

void test_froot()
{
	int i = 0, x = 0;
	
	char buf[32];
	fat_root_ent(1, buf);
	
	long fsize = fat_fsize(buf);
	
	disp_str("\n===========\n");
	disp_int(fsize);
	disp_str("                 \n===========");
	
	disp_str("\n===========\n");
	for (i = 0; i < 32; i++) {
		x = (int)buf[i];
		x = x & 0x0ff;
		disp_int(x);
		disp_str(((i + 1) % 8 == 0) ? "        \n" : "  ");
	}
	disp_str("===========\n");	
}

void test_frdf()
{
	char buf[32];
	char tbuf[512];
	/* seem the user stack is limited, 
	 * cannot have a buffer larger than 1520*/
	
	int i = 0, t = 0;
//	test_froot();
	
	i = get_root_ent_bname("BBB.TXT", buf);
	
//	fat_rdfile(tbuf, 3, i);
//	fcontent[3] = '\0';
	
	disp_str("==============                             \n");
	disp_int(i);
	disp_str("                                           \n");
	tbuf[3] = tbuf[4] = '\0';
	disp_str(tbuf);
	disp_str("                                           \n");
	for (i = 0; i < 3; i++) {
		t = (int)tbuf[i];
		t = t & 0x0ff;
		disp_int(t);
		disp_str(((i + 1) % 8 == 0) ? "           \n      " : "  ");
	}
	disp_str("                                           \n");
	disp_str("==============                             \n");
}

void test_ld2mem()
{
	int i = 0, t = 0;
	iFILE p_file = 0;
	long fsize = 0;
	char rbuf[32];
	
	char *xschedbuf = (char *)0x7e00;
	
	p_file = get_root_ent_bname("XMOD.ELF", rbuf);
	fsize = fat_fsize(rbuf);
	
	fat_rdfile(xschedbuf, fsize, p_file);
	
//	xschedbuf[652] = xschedbuf[653] = '\0';
//	disp_str(xschedbuf);
	disp_str("==============                             \n");
	disp_int(fsize);
	disp_str("                                           \n");
	for (i = 0; i < 3; i++) {
		t = (int)xschedbuf[i];
		t = t & 0x0ff;
		disp_int(t);
		disp_str(((i + 1) % 8 == 0) ? "           \n      " : "  ");
	}
	disp_str("                                           \n");
	
	disp_str("==============                             \n");
	
}


