/***************************************************************************
 * original Copyright (C) 2008 by Wenbo Yang 
   <solrex@gmail.com> <http://solrex.cn>

   This file is part of the source code of book "Write Your Own OS with Free
   and Open Source Software". Homepage @ <http://share.solrex.cn/WriteOS/>.

   This file is licensed under the GNU General Public License; either
   version 3 of the License, or (at your option) any later version.
 ***************************************************************************
 *   Modified by Cao, Chen - 2009                                          *
 *   ken.ccao@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

/* use this boot.bin to read metadata of hd */

/* real mode boot and loader */
.code16
.set    BaseOfStack,     0x7c00    /* Stack base address, inner */
.set    BaseOfLoader,    0x9000    /* Section loading address of LOADER.BIN */
.set    OffsetOfLoader,  0x0100    /* Loading offset of LOADER.BIN */

.text
/* floppy header of FAT12 */
    jmp     LABEL_START /* Start to boot. */
    nop                 /* nop required */

#include "fat12hdr.h"

/* initial registers. */
LABEL_START:
    mov     %cs,%ax
    mov     %ax,%ds
    mov     %ax,%es
    mov     %ax,%ss
    mov     $BaseOfStack, %sp

    /* clear screen */
    mov     $0x0600,%ax   /* %ah=6, %al=0 */
    mov     $0x0700,%bx   /* Black white */
    mov     $0,%cx        /* Top left: (0,0) */
    mov     $0x184f,%dx   /* Bottom right: (80,50) */
    int     $0x10         /* BIOS int 10h, ah=6: Initialize screen */
    
    /* Display "Booting**" */
    mov     $0,%dh
    call    DispStr       /* Display string(index 0)*/
    
/********************************************************************/
	xorl	%eax, %eax
	xorl	%eds, %eds

	movw	$0x0, %ax
	movw	%ax, %ds
	
	xorl	%eax, %eax
	xorl	%ebx, %ebx
	movl	$0, %ebx
	
//	mov	$0x41, %eax
	movl	$0x0104, %eax
	
//	lds	(%ebx, %eax, 4), %si
	lds	(%ebx, %eax), %si
	
// print
	push	%eax
	push    %ebx
	
	xorl	%eax, %eax
	xorl	%edx, %edx
	xorl	%ecx, %ecx
	movw	$0x10, %cx
	
PHDINF:	lodsb		/* load the vaule to %al */

	movb	%al, %dl	/* save a copy in %dl */
	
	// print high bits
	shrb	$4, %al
	andb	$0xf, %al
	addb	$0x30, %al
	
	mov     $0x0e,%ah
	mov     $0x0f,%bl   /* Front color: white */
	int     $0x10       /* BIOS int 10h, ah=0xe: Print char */
	
	// print low bits
	movb	%dl, %al
	andb	$0xf, %al
	addb	$0x30, %al
	
	mov     $0x0e,%ah
	mov     $0x0f,%bl   /* Front color: white */
	int     $0x10       /* BIOS int 10h, ah=0xe: Print char */
	
	xorl	%eax, %eax
	xorl	%edx, %edx
	loop	PHDINF
	
	pop     %ebx
	pop     %eax
// end print
	
	jmp	.
 
/********************************************************************/
/* ==================================================================
   variables
*/
wRootDirSizeForLoop:    .2byte  RootDirSectors
wSectorNo:              .2byte  0       /* sector number to read */
bOdd:                   .byte   0       /* is odd? */

/* ==================================================================
   String table
*/
.set		MessageLength,	9
BootMessage:	.ascii		"HDdrvinf_"	/* index 0 */

/* ==================================================================
   Routine: DispStr
   Action: Display a string, string index stored in %dh
*/
DispStr: 
    mov     $MessageLength, %ax
    mul     %dh                 /* the %dh (nth) msg in the structure */
    add     $BootMessage,%ax
    mov     %ax,%bp             /* es:bp -> the addr of the str */
    mov     %ds,%ax
    mov     %ax,%es             /* es:bp -> the addr of the str */
    mov     $MessageLength,%cx  /* String length */
    mov     $0x1301,%ax         /* ah = 0x13, al = 0x01(W) */
    mov     $0x07,%bx           /* PageNum 0(bh = 0), bw(bl= 0x07)*/
    mov     $0,%dl              /* Start row and column */
    int     $0x10               /* BIOS INT 10h, display string */
    ret



.org 510        /* go to address 0x510. */
.2byte 0xaa55   /* write boot flag to the end of 1st sector(512 bytes) */

