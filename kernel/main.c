
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "utils.h"


/*======================================================================*
                            kenos_main
 *======================================================================*/
PUBLIC int kenos_main()
{
//	unsigned int aint = *((unsigned int *)0x7c00);
//	disp_int(aint);
//	perror("");
//	kinfo("k_info");

	disp_str("----- init ata device -----\n");
//	ASSERT(1);
	init_ide_dev();
	disp_str("----- end of ata init -----\n");

//	int t = 0;
//	char buf[1024];
//	ide_rblks(0, 0, 2, buf);
	
	disp_str("-----\"kenos_main\" begins-----\n");

	TASK*		p_task;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	t_16		selector_ldt	= SELECTOR_LDT_FIRST;
	int		i;
	t_8		privilege;
	t_8		rpl;
	int		eflags;
	
	// init the proc_table
	for(i = 0; i< NR_TASKS + NR_PROCS; i++){
		if (i < NR_TASKS) {	/* tasks */
			p_task		= task_table + i;
			privilege	= PRIVILEGE_TASK;
			rpl		= RPL_TASK;
			eflags		= 0x1202;
			/* IF=1, IOPL=1, bit 2 is always 1 */
		}
		else {			/* user process */
			p_task		= user_proc_table + (i - NR_TASKS);
			privilege	= PRIVILEGE_USER;
			rpl		= RPL_USER;
			eflags		= 0x202;
			/* IF=1, bit 2 is always 1 */
		}

		strcpy(p_proc->name, p_task->name);	/* name of the process */
		p_proc->pid = i;			/* pid */

		p_proc->ldt_sel	= selector_ldt;
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
			sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		/* change the DPL */
		
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
			sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		/* change the DPL */
		
		p_proc->regs.cs = ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) |
							SA_TIL | rpl;
		p_proc->regs.ds = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) |
							SA_TIL | rpl;
		p_proc->regs.es = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) |
							SA_TIL | rpl;
		p_proc->regs.fs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) |
							SA_TIL | rpl;
		p_proc->regs.ss = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) |
							SA_TIL | rpl;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p_proc->regs.eip	= (t_32)p_task->initial_eip;
		p_proc->regs.esp	= (t_32)p_task_stack;
		p_proc->regs.eflags	= eflags;
		
		p_proc->nr_tty		= 0;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
	proc_table[0].ticks = proc_table[0].priority = 315;
	proc_table[1].ticks = proc_table[1].priority = 5;
	proc_table[2].ticks = proc_table[2].priority = 105;
	proc_table[3].ticks = proc_table[3].priority = 105;
	
	proc_table[1].nr_tty = 0;
	proc_table[2].nr_tty = 1;
	proc_table[3].nr_tty = 1;

	k_reenter = 0;
	ticks = 0;
	
	p_proc_ready = proc_table;
	
	init_clock();
	
	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
//	int i = 0;
//	sw_sched();
//	hd_intr();
//	printf("<xxxxxxhd: %x>\n", get_hd_info());
//	hd_xxx();
	ld_mod("XMOD.ELF");
	
	while(1){
//		disp_str("A.");
		printf("<Ticks: %x>", get_ticks());
//		sw_sched();
		milli_delay(100);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int i = 0;
	while(1){
//		disp_str("B.");
		printf("B");
		milli_delay(100);
	}
}


/*======================================================================*
                               TestC
 *======================================================================*/
void TestC()
{
	int i = 0;
	while(1){
//		disp_str("C.");
		printf("C");
		milli_delay(100);
	}
}


