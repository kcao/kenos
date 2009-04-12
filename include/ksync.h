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

#ifndef	_KENOS_KSYNC_H_
#define	_KENOS_KSYNC_H_

struct ksema {

	/* The current value of this semaphore.
	 * It represents the number of units of the 
	 * resource which are free. */
	unsigned int value;

	/* The ordered list of tasks waiting on this semaphore. */
//	struct task_struct *waiting_task_list_head;
};

/*
 * the kernel mutex is implemented as a binary semaphore.
 */
typedef struct ksema kmutex;


#endif /* _KENOS_KSYNC_H_ */


