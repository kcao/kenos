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

/* string.S */
.section .text

.type memcpy, @function
.globl	memcpy
/* void* memcpy(void* es:pDest, void* ds:pSrc, int iSize); */
memcpy:
	push	%ebp
	mov	%esp, %ebp
	
	push	%esi
	push	%edi
	push	%ecx
	
	mov	8(%ebp), %edi	/* dest */
	mov	12(%ebp), %esi	/* src */
	mov	16(%ebp), %ecx	/* counter */
.m_c1:
	cmp	$0, %ecx
	jz	.m_c2
	
	/* move byte after byte*/
	movb	%ds:(%esi), %al
	inc	%esi

	movb	%al, %es:(%edi)
	inc	%edi
	
	dec	%ecx
	jmp	.m_c1
.m_c2:
	mov	8(%ebp), %eax	/* the return value */
	
	pop	%ecx
	pop	%edi
	pop	%esi
	mov	%ebp, %esp
	pop	%ebp
	
	ret
	
