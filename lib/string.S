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

.globl	memcpy
.globl	memset
.globl	strcpy
.globl	strlen

/* void* memcpy(void* es:pDest, void* ds:pSrc, int iSize); */
.type memcpy, @function
memcpy:
	push	%ebp
	mov		%esp, %ebp
	
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


/* void memset(void* p_dst, char ch, int size); */
.type memset, @function
memset:
	push	%ebp
	mov	%esp, %ebp

	push	%esi
	push	%edi
	push	%ecx

	mov	8(%ebp), %edi	//Dest
	mov	12(%ebp), %edx	//char to be put
	mov	16(%ebp), %ecx	//counter
.m_s1:
	cmp	$0, %ecx
	jz	.m_s2

	movb	%dl, (%edi)
	inc	%edi

	dec	%ecx
	jmp	.m_s1
.m_s2:
	pop	%ecx
	pop	%edi
	pop	%esi
	mov	%ebp, %esp
	pop	%ebp

	ret


	
/* char* strcpy(char* p_dst, char* p_src); */
.type strcpy, @function
strcpy:
	push	%ebp
	mov	%esp, %ebp
	
	mov	12(%ebp), %esi	/* src */
	mov	8(%ebp), %edi	/* dest */
.s_c1:
	/* move byte by byte */
	mov	(%esi), %al
	inc	%esi
	movb	%al, (%edi)
	inc	%edi
	
	cmp	$0, %al		/* if encounter a '\0', exit */
	jnz	.s_c1
	
	mov	8(%ebp), %eax	/* the return value */
	
	pop	%ebp
	ret
/* the end of strcpy() */


/* int strlen(char* p_str); */
.type strlen, @function
strlen:
	push	%ebp
	mov	%esp, %ebp
	
	mov	$0, %eax	/* the initial value of len is zero*/
	mov	8(%ebp), %esi	/* %esi point to the head addr */
.sl1:
	cmpb	$0, (%esi)	/* if (%esi) == '\0'*/
	jz	.sl2		/* if (%esi) == '\0', then exit */
	inc	%esi		/* if (%esi) != '\0', then point to next char */
	inc	%eax		/*   and %eax++*/
	jmp	.sl1		/* loop */
.sl2:
	pop	%ebp
	ret
/* end ofstrlen() */
