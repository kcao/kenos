
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              protect.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


/* proto in this file */
PRIVATE void init_idt_desc(unsigned char vector, t_8 desc_type, 
		t_pf_int_handler handler, unsigned char privilege);
PRIVATE void init_descriptor(DESCRIPTOR * p_desc, t_32 base, 
		t_32 limit, t_16 attribute);


/* interrupt handles */
void	divide_error();
void	single_step_exception();
void	nmi();
void	breakpoint_exception();
void	overflow();
void	bounds_check();
void	inval_opcode();
void	copr_not_available();
void	double_fault();
void	copr_seg_overrun();
void	inval_tss();
void	segment_not_present();
void	stack_exception();
void	general_protection();
void	page_fault();
void	copr_error();
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();


/*======================================================================*
                            init_prot
 *----------------------------------------------------------------------*
	initialize IDT
 *======================================================================*/
PUBLIC void init_prot()
{
	init_8259A();

	// initial all of these to interrupt gate (no trap gate)
	init_idt_desc(INT_VECTOR_DIVIDE,	DA_386IGate, divide_error,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_DEBUG,		DA_386IGate, single_step_exception,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_NMI,		DA_386IGate, nmi,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_BREAKPOINT,	DA_386IGate, breakpoint_exception,	PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_OVERFLOW,	DA_386IGate, overflow,			PRIVILEGE_USER);
	init_idt_desc(INT_VECTOR_BOUNDS,	DA_386IGate, bounds_check,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_OP,	DA_386IGate, inval_opcode,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_NOT,	DA_386IGate, copr_not_available,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_DOUBLE_FAULT,	DA_386IGate, double_fault,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_SEG,	DA_386IGate, copr_seg_overrun,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_INVAL_TSS,	DA_386IGate, inval_tss,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_SEG_NOT,	DA_386IGate, segment_not_present,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_STACK_FAULT,	DA_386IGate, stack_exception,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PROTECTION,	DA_386IGate, general_protection,	PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_PAGE_FAULT,	DA_386IGate, page_fault,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_COPROC_ERR,	DA_386IGate, copr_error,		PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 0,	DA_386IGate, hwint00,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 1,	DA_386IGate, hwint01,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 2,	DA_386IGate, hwint02,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 3,	DA_386IGate, hwint03,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 4,	DA_386IGate, hwint04,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 5,	DA_386IGate, hwint05,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 6,	DA_386IGate, hwint06,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ0 + 7,	DA_386IGate, hwint07,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 0,	DA_386IGate, hwint08,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 1,	DA_386IGate, hwint09,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 2,	DA_386IGate, hwint10,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 3,	DA_386IGate, hwint11,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 4,	DA_386IGate, hwint12,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 5,	DA_386IGate, hwint13,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 6,	DA_386IGate, hwint14,			PRIVILEGE_KRNL);
	init_idt_desc(INT_VECTOR_IRQ8 + 7,	DA_386IGate, hwint15,			PRIVILEGE_KRNL);
	
	init_idt_desc(INT_VECTOR_SYS_CALL,	DA_386IGate, sys_call,			PRIVILEGE_USER);

	/* fill up the descriptor in GDT */
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = SELECTOR_KERNEL_DS;
	
	init_descriptor(&gdt[INDEX_TSS],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss),
			sizeof(tss) - 1,
			DA_386TSS);
	tss.iobase = sizeof(tss); /* there is no i/o permission bitmap */

	// fillup the every LDT of the process in GDT
	int i;
	PROCESS* p_proc	= proc_table;
	t_16 selector_ldt = INDEX_LDT_FIRST << 3;
	for(i=0; i<NR_TASKS + NR_PROCS; i++){
		init_descriptor(&gdt[selector_ldt >>3 ],
				vir2phys(seg2phys(SELECTOR_KERNEL_DS),
						proc_table[i].ldts),
				LDT_SIZE * sizeof(DESCRIPTOR) - 1,
				DA_LDT);
		p_proc++;
		selector_ldt += 1 << 3;
	}
}


/*======================================================================*
                             init_idt_desc
 *----------------------------------------------------------------------*
	initialize 386 interrupt gate
 *======================================================================*/
PUBLIC void init_idt_desc(unsigned char vector, t_8 desc_type, 
			t_pf_int_handler handler, unsigned char privilege)
{
	GATE *	p_gate	= &idt[vector];
	t_32	base	= (t_32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= SELECTOR_KERNEL_CS;
	p_gate->dcount		= 0;
	p_gate->attr		= desc_type | (privilege << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}

/*======================================================================*
                           seg2phys
 *----------------------------------------------------------------------*
 get the absolute addr by the seg
 *======================================================================*/
PUBLIC t_32 seg2phys(t_16 seg)
{
	DESCRIPTOR* p_dest = &gdt[seg >> 3];

	return	(p_dest->base_high << 24) |
		(p_dest->base_mid << 16) |
		(p_dest->base_low);
}

/*======================================================================*
                           init_descriptor
 *----------------------------------------------------------------------*
 *======================================================================*/
PRIVATE void init_descriptor(DESCRIPTOR * p_desc, t_32 base, t_32 limit, t_16 attribute)
{
	p_desc->limit_low	= limit & 0x0FFFF;	// limit1 (2-bytes)
	p_desc->base_low	= base & 0x0FFFF;	// base1 (2-bytes)
	p_desc->base_mid	= (base >> 16) & 0x0FF;	// base2 (1-byte)
	p_desc->attr1		= attribute & 0xFF;	// attribute1
	p_desc->limit_high_attr2= ((limit >> 16) & 0x0F) |
					(attribute >> 8) & 0xF0;
	// limit2 + attr2
	
	p_desc->base_high	= (base >> 24) & 0x0FF;	// base3 (1-byte)
}


/*======================================================================*
                            exception_handler
 *======================================================================*/
PUBLIC void exception_handler(int vec_no, int err_code, int eip,
				int cs, int eflags)
{
	int i;
	int text_color = 0x74; // fg: red, bg: gray

//	char err_description[][64] = {"",""...};//error when link! 

/*	ld(version 2.17) uses 'memset' to do the assignment, 
	and at here, we do not have the func, 
	if we still code like that, when gen KERNEL.BIN, 
	ld gives the error msg: 
		kernel/protect.o: In function `exception_handler':
		protect.c:(.text+0x52c): undefined reference to `memset'
		
	solution:
		coding as the following way, using an array of ptrs.
		this time, ld uses 'memcpy' to do the assignment,
		and coincidently we have defined the func ourselves.
		so it works. :-)
	
	note:
		maybe, this has something to do with my system,(???)
		if this kind of coding (using an array of str ptrs) 
		cannot make the link successful, 
		you may try the original way given by Forrest Yu.
*/

	char *err_description[20] = { "#DE Divide Error",
			"#DB RESERVED",
			"—  NMI Interrupt",
			"#BP Breakpoint",
			"#OF Overflow",
			"#BR BOUND Range Exceeded",
			"#UD Invalid Opcode (Undefined Opcode)",
			"#NM Device Not Available (No Math Coprocessor)",
			"#DF Double Fault",
			"    Coprocessor Segment Overrun (reserved)",
			"#TS Invalid TSS",
			"#NP Segment Not Present",
			"#SS Stack-Segment Fault",
			"#GP General Protection",
			"#PF Page Fault",
			"—  (Intel reserved. Do not use.)",
			"#MF x87 FPU Floating-Point Error (Math Fault)",
			"#AC Alignment Check",
			"#MC Machine Check",
			"#XF SIMD Floating-Point Exception"
			};

	/* print whitespaces to clean the screen, and set 'disp_pos' to 0 */
	disp_pos = 0;
	for(i=0; i< 80 * 5; i++){
		disp_str(" ");
	}
	disp_pos = 0;

	disp_color_str("Exception! --> ", text_color);
	disp_color_str(err_description[vec_no], text_color);
	disp_color_str("\n\n", text_color);
	disp_color_str("EFLAGS:", text_color);
	disp_int(eflags);
	disp_color_str("CS:", text_color);
	disp_int(cs);
	disp_color_str("EIP:", text_color);
	disp_int(eip);

	if(err_code != 0xFFFFFFFF){
		disp_color_str("Error code:", text_color);
		disp_int(err_code);
	}

}
