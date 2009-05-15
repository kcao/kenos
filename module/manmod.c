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
/*
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
*/

#include "utils.h"
#include "fs/fattabe.h"
#include "fs/fatroot.h"
#include "fs/fatrdfile.h"
#include "mod/modconst.h"
#include "mod/modtypes.h"
#include "proc.h"	/* for PROCESS */


void xmod_ld2mem(char *mod_name)
{
	int i = 0, t = 0;
	iFILE p_file = 0;
	long fsize = 0;
	char rbuf[32];
	
	char *xschedbuf = (char *)BaseOfModFilePhyAddr;
	
	p_file = get_root_ent_bname(mod_name, rbuf);
	fsize = fat_fsize(rbuf);
	
	fat_rdfile(xschedbuf, fsize, p_file);
}

int xmod_lay2dst()
{
	int i = 0;
	int phNr = 0, phOffset = 0, phSize = 0;
	char *modfaddr = (char *)BaseOfModFilePhyAddr;
	char *modvaddr = (char *)modfaddr + 8;
	int pType = 0, pFilesz = 0, pOffset = 0;
	char *phAddr = 0;
	char *pvaddr = 0;
	
	phNr = mem2num(modfaddr + ELFphnumOFFSET, ELFphnumLEN);
	phSize = mem2num(modfaddr + ELFphentsizeOFFSET, ELFphentsizeLEN);
	phOffset = mem2num(modfaddr + ELFphoffOFFSET, ELFphoffLEN);
	
	phAddr = (char *)(BaseOfModFilePhyAddr + phOffset);
//	
	for (i = 0; i < phNr; i++) {
		pType = mem2num(phAddr + PHtypeOFFSET, PHtypeLEN);
		if (pType == 0) {
			continue;
		}
		
		pOffset = mem2num(phAddr + PHoffsetOFFSET, PHoffsetLEN);
		pvaddr = (char *)mem2num(phAddr + PHvaddrOFFSET, PHvaddrLEN);
		pFilesz = mem2num(phAddr + PHfileszOFFSET, PHfileszLEN);
		memcpy(pvaddr, modfaddr + pOffset, pFilesz);
		
		phAddr += phSize;
	}
	
//	return (int)pvaddr;
	return 0;
}


void xmod_test(char *fname)
{
	int i = 0, t = 0;
	void (*nullxmod)(PROCESS *, PROCESS **) = (xmodfunc)XModPhyAddr;
//	void (*nullxmod)(int) = (xmodfunc)XModPhyAddr;
	PROCESS *p = 0;
	
	char *xschedbuf = (char *)BaseOfModFilePhyAddr;
	
//	xmod_ld2mem("XMOD.ELF");
	xmod_ld2mem(fname);
	t = xmod_lay2dst();
/*==============================
	nullxmod(p, &p);
 *==============================*/
/*	disp_str("==============                             \n");
	disp_int(t);
	disp_str("                                           \n");
//	for (i = 0; i < 3; i++) {
//		t = (int)xschedbuf[i];
//		t = t & 0x0ff;
//		disp_int(t);
//		disp_str(((i + 1) % 8 == 0) ? "           \n      " : "  ");
//	}
	disp_str("                                           \n");
	
	disp_str("==============                             \n");
*/

}

/* PUBLIC */
int sys_ld_mod(char *fname)
{
	xmod_test(fname);
//	disp_str("==============                             \n");
	
	return 0;
}

/*
PUBLIC int manmod()
{
	smodf();
	
	disp_str("xxmod");
	
	return 0;
}
*/


