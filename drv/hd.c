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

/* obsolete */

#include "asm/io.h"
#include "asm/system.h"
#include "kenos/config.h"
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "drv/hdreg.h"
#include "drv/blk.h"
#include "drv/hd.h"

#include "fs/fattest.h"

#define MAX_HD	2

#define CMOS_READ(addr) ({ \
	outb_p(0x80|addr,0x70); \
	inb_p(0x71); \
})



#define port_read(port,buf,nr) \
	__asm__("cld;rep;insw"::"d" (port),"D" (buf),"c" (nr):"cx","di")

#define port_write(port,buf,nr) \
	__asm__("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr):"cx","si")

void unexpected_hd_intr()
{
	printf("Unexpected HD interrupt\n\r");
}

static int is_drv_busy(void)
{
	unsigned int i;
	
	for (i = 0; i < 10000; i++) {
		if (READY_STAT == (inb_p(HD_STATUS) & (BUSY_STAT|READY_STAT)))
			break;
	}

	i = inb(HD_STATUS);
	i &= BUSY_STAT | READY_STAT | SEEK_STAT;
	if (i == READY_STAT | SEEK_STAT)
		return(0);
	
	printf("HD controller times out\n\r");
	return(1);
}


/*  */
PUBLIC int sys_get_hd_info()
{
	int i,drive;
	unsigned char cmos_disks;
	struct partition *p;

	int hd_nr = 0;
	
	if ((cmos_disks = CMOS_READ(0x12)) & 0xf0) {
		if (cmos_disks & 0x0f)
			hd_nr = 2;
		else
			hd_nr = 1;
	}
	else {
		hd_nr = 0;
	}
	
	return hd_nr;
}


PUBLIC void sys_hd_xxx()
{
//	char buf[1024];
	
//	unsigned int t = 0;
	
//	test_ftab();
//	test_froot();
	test_frdf();

/*	t = ide_rblks(0, 0, 2, buf);
	disp_str("good reading: ");
	t = 1;
	disp_int(t);

	disp_int(buf[510]);
	disp_str(" \n");
	disp_int(buf[511]);
	disp_str(" \n");
	disp_int(buf[512]);
	disp_str(" \n");
	disp_int(buf[513]);
	disp_str(" \n");
*/
	
}

