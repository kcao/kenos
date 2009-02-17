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
 
/* ==================================================================
   Routine: DispAL
   Action: Display the number in %al
*/
DispAL:
	push	%ecx
	push	%edx
	push	%edi
	
	mov	(dwDispPos), %edi
	
	mov	$0x0f, %ah	/*0b0000: black bg, 0b1111: white fg*/
	mov	%al, %dl
	shr	$4, %al
	mov	$2, %ecx
.DAbegin:
	and	$0b1111, %al
	cmp	$9, %al
	ja	.DA1
	add	$'0', %al
	jmp	.DA2
.DA1:
	sub	$0x0a, %al
	add	$'A', %al
.DA2:
	mov	%ax, %gs:(%edi)
	add	$2, %edi
	
	mov	%dl, %al
	loop	.DAbegin
/*	add	$2, %edi	*/
	
	mov	%edi, (dwDispPos)
	
	pop	%edi
	pop	%edx
	pop	%ecx
	
	ret
/* the end of DispAL */
	

/* ==================================================================
   Routine: DispInt
   Action: Display a integer
*/
DispInt:
	mov	4(%esp), %eax
	shr	$24, %eax
	call	DispAL
	
	mov	4(%esp), %eax
	shr	$16, %eax
	call	DispAL
	
	mov	4(%esp), %eax
	shr	$8, %eax
	call	DispAL
	
	mov	4(%esp), %eax
	call	DispAL
	
	mov	$0x07, %ah	/*0b0000: black bg, 0b0111: gray fg*/
	mov	$'h', %al
	push	%edi
	mov	(dwDispPos), %edi
	mov	%ax, %gs:(%edi)
	add 	$4, %edi
	mov	%edi, (dwDispPos)
	pop 	%edi
	
	ret
/* the end of DispInt */


/* ==================================================================
   Routine: DispStr
   Action: Display a string
*/
DispStr:
	push	%ebp
	mov	%esp, %ebp
	push	%ebx
	push	%esi
	push	%edi
	
	mov	8(%ebp), %esi	/* pszInfo */
	mov	(dwDispPos), %edi
	mov	$0x0f, %ah
.DS1:
	lodsb
	test	%al, %al
	jz	.DS2
	cmp	$0x0a, %al	/* a return? */
	jnz	.DS3
	push	%eax
	mov	%edi, %eax
	mov	$160, %bl
	div	%bl
	and	$0xff, %eax
	inc	%eax
	
	mov	$160, %bl
	mul	%bl
	mov	%eax, %edi
	pop	%eax
	jmp	.DS1	
.DS3:
	mov	%ax, %gs:(%edi)
	add	$2, %edi
	jmp	.DS1
.DS2:
	mov	%edi, (dwDispPos)
	
	pop	%edi
	pop	%esi
	pop	%ebx
	pop	%ebp

	ret
/* the end of DispStr */


/* ==================================================================
   Routine: DispReturn
   Action: Display a return symbol
*/
DispReturn:
	push	$szReturn
	call	DispStr
	add	$4, %esp
	
	ret
/* the end of DispReturn */


/* ==================================================================
   Routine: MemCpy
   Action: void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
*/
MemCpy:
	push	%ebp
	mov	%esp, %ebp
	
	push	%esi
	push	%edi
	push	%ecx
	
	mov	8(%ebp), %edi	/* dest */
	mov	12(%ebp), %esi	/* src */
	mov	16(%ebp), %ecx	/* counter */
.MC1:
	cmp	$0, %ecx
	jz	.MC2
	
	/* move byte after byte*/
	movb	%ds:(%esi), %al
	inc	%esi

	movb	%al, %es:(%edi)
	inc	%edi
	
	dec	%ecx
	jmp	.MC1
.MC2:
	mov	8(%ebp), %eax	/* the return value */
	
	pop	%ecx
	pop	%edi
	pop	%esi
	mov	%ebp, %esp
	pop	%ebp
	
	ret
/* the end of MemCpy */

