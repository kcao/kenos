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

#include "const.h"
#include "asm/io.h"
#include "drv/hdreg.h"

#define MAX_HD	2

#define CMOS_READ(addr) ({ \
	outb_p(0x80|addr,0x70); \
	inb_p(0x71); \
})

/*
 *  This struct defines the HD's and their types.
 */
struct hd_info_struct {
	int head,sect,cyl,wpcom,lzone,ctl;
};

#ifdef HD_TYPE
	struct hd_info_struct hd_info[] = { HD_TYPE };
	#define NR_HD ((sizeof (hd_info))/(sizeof (struct hd_info)))
#else
	struct hd_info_struct hd_info[] = { {0,0,0,0,0,0},{0,0,0,0,0,0} };
	static int NR_HD = 0;
#endif

static struct hd_struct {
	long start_sect;
	long nr_sects;
	
} hd[5*MAX_HD]={{0,0},};

#define port_read(port,buf,nr) \
	__asm__("cld;rep;insw"::"d" (port),"D" (buf),"c" (nr):"cx","di")

#define port_write(port,buf,nr) \
	__asm__("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr):"cx","si")


/* actually this func should be called only once */
PUBLIC int sys_init_hd()
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
	
	printf("xxxxxxxxxxx");
	return hd_nr;
}


