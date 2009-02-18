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

/* klib.S */

#include "sconst.h"
/* disp_pos is defined in gobal.h as int */

.section .text

.globl	disp_str
.globl	disp_color_str
.globl	out_byte
.globl	in_byte
.globl	enable_irq
.globl	disable_irq
.globl	enable_int
.globl	disable_int

.type disp_str, @function
/* void disp_str(char * info); */
disp_str:
	push	%ebp
	mov	%esp, %ebp
	
	mov	8(%ebp), %esi	/* pszInfo */
	mov	(disp_pos), %edi
	mov	$0x0f, %ah
.d_s1:
	lodsb
	test	%al, %al
	jz	.d_s2
	cmp	$0x0a, %al	/* a return? */
	jnz	.d_s3
	
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
	jmp	.d_s1	
.d_s3:
	mov	%ax, %gs:(%edi)
	add	$2, %edi
	jmp	.d_s1
.d_s2:
	mov	%edi, (disp_pos)
	
	pop	%ebp

	ret
	
.type disp_color_str, @function
/* void disp_color_str(char * info, int color); */
disp_color_str:
	push	%ebp
	mov	%esp, %ebp
	
	mov	8(%ebp), %esi	/* pszInfo */
	mov	(disp_pos), %edi
	mov	12(%ebp), %ah	/* color */
.d_c_s1:
	lodsb
	test	%al, %al
	jz	.d_c_s2
	cmp	$0x0a, %al	/* a return? */
	jnz	.d_c_s3
	
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
	jmp	.d_c_s1	
.d_c_s3:
	mov	%ax, %gs:(%edi)
	add	$2, %edi
	jmp	.d_c_s1
.d_c_s2:
	mov	%edi, (disp_pos)

	pop	%ebp
	
	ret


.type out_byte, @function
/* void out_byte(t_port port, t_8 value); */
out_byte:
	mov	4(%esp), %edx	/* prot */
	mov	8(%esp), %al	/* value */
	out	%al, %dx
	nop
	nop
	nop
	ret


.type in_byte, @function
/* t_8 in_byte(t_port port); */
in_byte:
	mov	4(%esp), %edx	/* port */
	xor	%eax, %eax
	in	%dx, %al
	nop
	nop
	nop

	ret


.type disable_irq, @function
/* void disable_irq(int irq); */
/* Disable an interrupt request line by setting an 8259 bit.
  Equivalent code for irq < 8:
	out_byte(INT_CTLMASK, in_byte(INT_CTLMASK) | (1 << irq));
  Returns true if the interrupt was not already disabled.
*/
disable_irq:
	mov	4(%esp), %ecx	/* irq */
	pushf
	cli
	mov	$1, %ah
	rolb	%cl, %ah	/* %ah = (1 << (irq % 8)) */
	cmpb	$8, %cl
	jae	disable_8	/* disable irq >= 8 at the slave 8259 */
disable_0:
	inb	$INT_M_CTLMASK, %al
	testb	%ah, %al
	jnz	dis_already	/* already disabled? */

	orb	%ah, %al
	outb	%al, $INT_M_CTLMASK	/* set bit at master 8259 */
	popf
	movl	$1, %eax	/* the return value? disabled by this func */
	
	ret
disable_8:
	inb	$INT_M_CTLMASK, %al
	testb	%ah, %al
	jnz	dis_already
	
	orb	%ah, %al
	outb	%al, $INT_S_CTLMASK	/* set bit at slave 8259 */
	popf
	movl	$1, %eax	/* the return value? */
	
	ret
dis_already:
	popf
	xorl	%eax, %eax
	ret


.type enable_irq, @function
/* void enable_irq(int irq); */
/* Enable an interrupt request line by clearing an 8259 bit.
  Equivalent code:
	if(irq < 8){
		out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) & ~(1 << irq));
	}
	else{
		out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) & ~(1 << irq));
	}
*/
enable_irq:
	movl	4(%esp), %ecx	/* the value of irq */
	pushf
	cli
	movb	$~1, %ah
	rolb	%cl, %ah	/* %ah = ~(1 << (irq % 8)) */
	cmp	$8, %cl
	jae	enable_8	/* enable irq >= 8 at the slave 8259 */
enable_0:
	inb	$INT_M_CTLMASK, %al
	andb	%ah, %al
	outb	%al, $INT_M_CTLMASK	/* clear bit at master 8259 */
	popf
	
	ret
enable_8:
	inb	$INT_S_CTLMASK, %al
	andb	%ah, %al
	outb	%al, $INT_S_CTLMASK	/* clear bit at slave 8259 */
	popf
	
	ret


.type disable_int, @function
/* void disable_int(); */
disable_int:
	cli
	ret

.type enable_int, @function
/* void enable_int(); */
enable_int:
	sti
	ret

