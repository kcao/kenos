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
 
/* kernel.S */

.set	SELECTOR_KERNEL_CS, 8

/* 
// globl funcs
extern	cstart
extern	exception_handler
extern	spurious_irq

// globl variables
extern	gdt_ptr
extern	idt_ptr
extern	disp_pos
*/

.code32

.section .bss
StackSpace:
	.rept   (2048)
	.byte   0
	.endr
StackTop:

.section .text

.globl _start

.globl	divide_error
.globl	single_step_exception
.globl	nmi
.globl	breakpoint_exception
.globl	overflow
.globl	bounds_check
.globl	inval_opcode
.globl	copr_not_available
.globl	double_fault
.globl	copr_seg_overrun
.globl	inval_tss
.globl	segment_not_present
.globl	stack_exception
.globl	general_protection
.globl	page_fault
.globl	copr_error
.globl	hwint00
.globl	hwint01
.globl	hwint02
.globl	hwint03
.globl	hwint04
.globl	hwint05
.globl	hwint06
.globl	hwint07
.globl	hwint08
.globl	hwint09
.globl	hwint10
.globl	hwint11
.globl	hwint12
.globl	hwint13
.globl	hwint14
.globl	hwint15

_start:
/*	mov	$0xb800,%ax
	movw	%ax,%gs		
	mov	$0x0f, %ah
	mov	$'K',%al
	mov	%ax,%gs:((80*1+39)*2)
	jmp	.
*/

/* set the stack ptr to bss seg from Loader */
	movl	$StackTop, %esp
	movl	$0, (disp_pos)

	sgdt	gdt_ptr
	call	cstart
	lgdtw	gdt_ptr

	lidtw	idt_ptr

	jmp	$SELECTOR_KERNEL_CS, $csinit
csinit:
/*	push	$0
	popfl	*/	/* Pop top of stack into EFLAGS */
	
/* error opcodes to make exceptions */
//	jmp $0x40, $0	
//	ud2		//undefined interrupt */
	
	sti
	
	hlt


/* ----------- */
.macro hwint_master Irq
	push	\Irq
	call	spurious_irq
	add	$4, %esp
	hlt
.endm


.align 16
hwint00:		// Interrupt routine for irq 0 (the clock).
	hwint_master	0

.align 16
hwint01:		// Interrupt routine for irq 1 (keyboard)
	hwint_master	1

.align 16
hwint02:		// Interrupt routine for irq 2 (cascade!)
	hwint_master	2

.align 16
hwint03:		// Interrupt routine for irq 3 (second serial)
	hwint_master	3

.align 16
hwint04:		// Interrupt routine for irq 4 (first serial)
	hwint_master	4

.align 16
hwint05:		// Interrupt routine for irq 5 (XT winchester)
	hwint_master	5

.align 16
hwint06:		// Interrupt routine for irq 6 (floppy)
	hwint_master	6

.align 16
hwint07:		// Interrupt routine for irq 7 (printer)
	hwint_master	7



.macro hwint_slave Irq
	push	\Irq
	call	spurious_irq
	add		$4, %esp
	hlt
.endm


.align 16
hwint08:		// Interrupt routine for irq 8 (realtime clock).
	hwint_slave	8

.align 16
hwint09:		// Interrupt routine for irq 9 (irq 2 redirected)
	hwint_slave	9

.align 16
hwint10:		// Interrupt routine for irq 10
	hwint_slave	10

.align 16
hwint11:		// Interrupt routine for irq 11
	hwint_slave	11

.align 16
hwint12:		// Interrupt routine for irq 12
	hwint_slave	12

.align 16
hwint13:		// Interrupt routine for irq 13 (FPU exception)
	hwint_slave	13

.align 16
hwint14:		// Interrupt routine for irq 14 (AT winchester)
	hwint_slave	14

.align 16
hwint15:		// Interrupt routine for irq 15
	hwint_slave	15



/* interrupts and exceptions */
divide_error:
	push	$0xFFFFFFFF	//; no err code
	push	$0		//; vector_no	= 0
	jmp	exception
single_step_exception:
	push	$0xFFFFFFFF	//; no err code
	push	$1		//; vector_no	= 1
	jmp	exception
nmi:
	push	$0xFFFFFFFF	//; no err code
	push	$2		//; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	$0xFFFFFFFF	//; no err code
	push	$3		//; vector_no	= 3
	jmp	exception
overflow:
	push	$0xFFFFFFFF	//; no err code
	push	$4		//; vector_no	= 4
	jmp	exception
bounds_check:
	push	$0xFFFFFFFF	//; no err code
	push	$5		//; vector_no	= 5
	jmp	exception
inval_opcode:
	push	$0xFFFFFFFF	//; no err code
	push	$6		//; vector_no	= 6
	jmp	exception
copr_not_available:
	push	$0xFFFFFFFF	//; no err code
	push	$7		//; vector_no	= 7
	jmp	exception
double_fault:
	push	$8		//; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	$0xFFFFFFFF	//; no err code
	push	$9		//; vector_no	= 9
	jmp	exception
inval_tss:
	push	$10		//; vector_no	= A
	jmp	exception
segment_not_present:
	push	$11		//; vector_no	= B
	jmp	exception
stack_exception:
	push	$12		//; vector_no	= C
	jmp	exception
general_protection:
	push	$13		//; vector_no	= D
	jmp	exception
page_fault:
	push	$14		//; vector_no	= E
	jmp	exception
copr_error:
	push	$0xFFFFFFFF	//; no err code
	push	$16		//; vector_no	= 10h
	jmp	exception

exception:
	call	exception_handler
	add	$8, %esp	// 'add	esp, 4*2' 
	// let the top of stack point to EIP
	// then the stack will look like this: form top to bottom, 
	// EIP、CS、EFLAGS
	hlt


