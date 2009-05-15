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


.section .text

.globl _start

/* xsched(PROCESS *proc_t, xx) */
_start:
	nop
	nop
	jmp	xmod_entry

#include "lib.h"

xmod_entry:
	pushl	%ebp
	movl	%esp, %ebp
	
//	movl	8(%ebp), %eax
//	movb	$1, %al
//	call	DispAL
	
//	movl	$3, %eax
	xorl	%eax, %eax
	movl	12(%ebp), %eax	/* argv[2] */
	pushl	%eax
	xorl	%eax, %eax
	movl	8(%ebp), %eax	/* argv[1] */
	pushl	%eax
	call	xsched01	/* argv[0] */
	addl	$8, %esp
	
//	movb	$2, %al
//	call	DispAL
//	jmp	.
	
	movl	%ebp, %esp
	popl	%ebp
	ret


