
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
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


/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void sched0(PROCESS *proc_t, PROCESS **p_ready)
{
	PROCESS	*p;
	int	greatest_ticks = 0;

	while ( 0 == greatest_ticks ) {
		for (p = proc_t; p < proc_t + NR_TASKS + NR_PROCS; 
			p++) {
			
			if (p->ticks > greatest_ticks) {
				greatest_ticks = p->ticks;
			//	p_proc_ready = p;
				(*p_ready) = p;
			}
		}

		if (greatest_ticks == 0) {
			for (p = proc_t; 
				p < proc_t + NR_TASKS + NR_PROCS; 
				p++) {
				
				p->ticks = p->priority;
			}
		}
	}
}

PUBLIC void sched01(PROCESS *proc_t, PROCESS **p_ready)
{}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
//	disp_str("+");
	return ticks;
}


