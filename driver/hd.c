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

#include "asm/io.h"
#include "asm/system.h"
#include "drv/hdreg.h"
#include "drv/blk.h"
#include "kenos/config.h"
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

#define MAX_HD	2

#define CMOS_READ(addr) ({ \
	outb_p(0x80|addr,0x70); \
	inb_p(0x71); \
})

/*
 *  This struct defines the HD's and their types.
 */
struct hd_info_struct {
	int head, sect, cyl, wpcom, lzone, ctl;
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

void unexpected_hd_intr()
{
	printf("Unexpected HD interrupt\n\r");
}

static int controller_ready(void)
{
	int retries=10000;

	while (--retries && (inb_p(HD_STATUS)&0xc0)!=0x40)
		;
	
	return (retries);
}

static int win_result(void)
{
	int i=inb_p(HD_STATUS);
 	
 	if (
 		(i & 	(BUSY_STAT | READY_STAT 
 			| WRERR_STAT | SEEK_STAT | ERR_STAT)
 		) 
 		== 
 		(READY_STAT | SEEK_STAT)
 	) {
		return(0); /* ok */ 
	}
	
	if (i&1) i=inb(HD_ERROR)
		;
	
	return (1);
}



/* 
 * drive - Nr of hd, 
 * nsect - , sect - start sect Nr
 * head - , cyl - 
 * cmd - cmd code (refer to include/drv/hdreg.h)
 * intr_addr - intr processing func
 * 
 */
static void hd_out(unsigned int drive, 
		unsigned int nsect, unsigned int sect, 
		unsigned int head, unsigned int cyl, 
		unsigned int cmd, 
		void (*intr_addr)(void))
{
	register int port asm("dx");


	if (drive>1 || head>15) {
		panic("trying to write bad sector");
	}
	if (!controller_ready()) {
		panic("HD controller not ready");
	}
	
	do_hd = intr_addr;
	outb_p(hd_info[drive].ctl, HD_CMD);
	port = HD_DATA;
	outb_p(hd_info[drive].wpcom >> 2, ++port);
	outb_p(nsect, ++port);
	outb_p(sect, ++port);
	outb_p(cyl, ++port);
	outb_p(cyl >> 8, ++port);
	outb_p(0xA0 | (drive << 4) | head, ++port);
	outb(cmd, ++port);

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

static void reset_controller(void) 
{
	int i;
	
	outb(4, HD_CMD);
	
	for(i = 0; i < 100; i++) { 
		nop();
	}
	
	outb(hd_info[0].ctl & 0x0f, HD_CMD);
	
	if (is_drv_busy()) {
		printf("HD-controller still busy\n\r");
	}
	
	if ((i = inb(HD_ERROR)) != 1) {
		printf("HD-controller reset failed: %x\n\r",i);
	}

	
}

static void recal_intr(void)
{
/* unimplemented */
}

/* reset the nr-th hd */
static void reset_hd(int nr)
{
	reset_controller();
	
	hd_out(nr, 
		hd_info[nr].sect, hd_info[nr].sect, 
		hd_info[nr].head-1, hd_info[nr].cyl, 
		WIN_SPECIFY, &recal_intr);

}


PUBLIC void hd_intr(int irq)
{
//	outb_p(0x20, 0xA0);	/* send EOI to interrupt controller #1 */
	
//	outb(0x20, 0x20);	/* send EOI to 8259A master intr controller */
				/* to end hardware intr */
	
	int a = 0, b = 0, rdNr = 256, dataReg = 0x1f0;
	char buff[512];
	
	/* read until device has data for us */
	__asm__ ("xorl	%edx, %edx\n\t"
		"xorl	%eax, %eax\n\t"
		"movw	$0x1f7, %dx\n"
		"LOOP5:\t"
		"inb	%dx, %al\n\t"
		"andb	$0x08, %al\n\t" /* 00001000b */
					/* if DRQ is not set, 
					 * the device doesn't 
					 * have data for us. */
		"je	LOOP5\n\t"	/* yet, so keep looking 
					 * until it does! */
		);
//	printf("hit: data\n");
	
	__asm__ __volatile__(
		"cld\n\t"
		"rep\n\t"
		"insw"
		: : "c"(rdNr), "d"(dataReg), "D"(buff)
		);
//	printf("hit: read\n");
	
	
	
//	printf("hd_intr_handler: %x\n", irq);
/*	if (do_hd == NULL) {
		unexpected_hd_intr();
		return ;
	}
	
	do_hd();
*/
}

/* actually this func should be called only once 
 * at system getting up. */
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

PUBLIC void init_hd()
{
//	blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST;

//	set_intr_gate(0x2E,&hd_interrupt);
	printf("init_hd\n");

	put_irq_handler(AT_WINI_IRQ, hd_intr);
	 
	// enable_irq
	// reset the mask bit attached to 8259A master int2
	outb_p(inb_p(0x21)&0xfb, 0x21); 
	// reset slave hd intr request mask bit
	outb(inb_p(0xA1)&0xbf, 0xA1);
	
}

static void bad_rw_intr(void)
{}

static void read_intr()
{
	if (win_result()) {
		bad_rw_intr();
		return;
	}
//	port_read(HD_DATA,CURRENT->buffer,256);

}


PUBLIC void ident_dev()
{
//	printf("ident_dev\n");
	int a = 0, b = 0, rdNr = 256, dataReg = 0x1f0;
	char buff[512];
	
	/* make sure hd is NOT busy */
	__asm__ ("xorl	%edx, %edx\n\t"
		"xorl	%eax, %eax\n\t"
		"movw	$0x1f7, %dx\n"
		"LOOP1:\n\t"
		"inb	%dx, %al\n\t"
		"andb	$0x80, %al\n\t"
		"jne	LOOP1\n\t"
		);
//	printf("hit: bsy\n");

//	__asm__("movl	%%eax, %0"
//		: "=r" (b)	/* output */
//		:"r"(a)		/* input */
//		:"%eax"		/* clobbered register */
//		);
//	printf("eax = %x\n", b);
	
	/* Clear interrupts so something doesn't interrupt 
	 * the drive or controller while this program is running.
	 */
	__asm__("cli\n\t");	//disable_int()
//	printf("hit: cli\n");
	
	/* make sure hd is DRDY */
	__asm__ ("xorl	%edx, %edx\n\t"
		"xorl	%eax, %eax\n\t"
		"movw	$0x1f7, %dx\n"
		"LOOP2:\t"
		"inb	%dx, %al\n\t"
		"andb	$0x40, %al\n\t" /* 01000000b */
	//	"je	LOOP2\n\t"
		);
//	printf("hit: drdy\n");
	
	/* send "IDENTIFY DRIVE" command. */
	__asm__ ("xorl	%edx, %edx\n\t"
		"xorl	%eax, %eax\n\t"
		"movw	$0x1f6, %dx\n\t"/* device/head register */
		"movb	$0x0, %al\n\t"
		"outb	%al, %dx\n\t"	/* select device 0 (master)*/
					/* 0x10 slave */
		"xorl	%edx, %edx\n\t"
		"xorl	%eax, %eax\n\t"
		"movw	$0x1f7, %dx\n\t"/* command register */
		"movb	$0xec, %al\n\t"	/* "IDENTIFY DRIVE" command */
		"outb	%al, %dx\n\t"	/* sends the command! */
		);
//	printf("hit: id\n");
//	
	
	/* read until device has data for us */
//	__asm__ ("xorl	%edx, %edx\n\t"
//		"xorl	%eax, %eax\n\t"
//		"movw	$0x1f7, %dx\n"
//		"LOOP3:\t"
//		"inb	%dx, %al\n\t"
//		"andb	$0x08, %al\n\t" /* 00001000b */
//					/* if DRQ is not set, 
//					 * the device doesn't 
//					 * have data for us. */
//		"je	LOOP3\n\t"	/* yet, so keep looking 
//					 * until it does! */
//		);
	
//	printf("hit: data\n");
	
//	while (1) {}
	
	/* read the result */
//	__asm__("movw	$0x1f0, %%dx\n\t");/* data register */

/*	__asm__ __volatile__(
		"cld\n\t"
		"rep\n\t"
		"insw"
		: : "c"(rdNr), "d"(dataReg), "D"(buff)
		);
	printf("hit: read\n");
*/

//	printf(buff);
	
//	__asm__("movl	%%edx, %0"
//		: "=r" (b)	/* output */
//		:"r"(a)		/* input */
//		:"%eax"		/* clobbered register */
//		);
	/* ;We now have the string data in buff, 
	 * so let's re-enable interrupts. 
	 */
	__asm__("sti\n\t");	// enable_int();
	
	printf("edx = %x\n", b);
	
}

PUBLIC void sys_hd_xxx()
{
//	printf("\nis_drv_bsy: %x\n", is_drv_busy());
	ident_dev();
//	printf("hd info: %x\n", hd_info[0].cyl);
}

