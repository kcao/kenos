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

#include "ksync.h"
#include "asm/system.h"

/*
 * initializes the semaphore with the specified integer value.
 */
int ksema_init(struct ksema *sema, unsigned int initval)
{
	if (sema)
		sema->value = initval;
	return 0;
}

/*
 * Implements the P (down) operation on the specified kernel semaphore.
 * Do not call this function from an interrupt handler!
 */
int ksema_p(struct ksema *sem)
{
	unsigned long eflags;

	disable_hwint(eflags);

//	if (!sem->value) {
//		/* Append the current task to the semaphore's wait queue 
//		 * and sleep. */
//		list_append(sem->waiting_task_list_head, current);
//		current->state = TASK_UNINTERRUPTIBLE;
//		schedule();
//	}

	/* Decrement the value of the semaphore. */
	sem->value--;
	
	restore_hwint(eflags);
	
	if (sem->value < 0) {
		sem->value++;
		return -1;
	}
	return 0;
}

/*
 * Implements the V (up) operation on the specified kernel semaphore.
 */
int ksema_v(struct ksema *sem)
{
	unsigned long eflags;
//	struct task_struct *t;

	disable_hwint(eflags);

	/* Increment the value of the semaphore. */
	sem->value++;

	/* Remove the first task from the semaphore's wait queue and 
	 * wake it up! */
//	if (!list_empty(sem->waiting_task_list_head)) {
//		t = list_pop_head(sem->waiting_task_list_head);
//		t->state = TASK_RUNNABLE;
//	}

	restore_hwint(eflags);
	
	return 0;
}


/*
 * initializes the kmetux in the unlocked state.
 */
int kmutex_init(struct_kmutex * mutex)
{
	return ksema_init(mutex, 1);
}

/*
 * Locks the specified mutex.
 * Do not call this function from an interrupt handler!
 */
int kmutex_lock(struct_kmutex *mut)
{
	return ksema_p((struct ksema *) mut);
}

/*
 * Unlocks the specified mutex.
 */
int kmutex_unlock(struct_kmutex *mut)
{
	return ksema_v((struct ksema *) mut);
}

