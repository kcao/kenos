/***************************************************************************
 *   Copyright (C) 2009 by Cao, Chen                                       *
 *   ken.ccao@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include "proc.h"

void xsched01(PROCESS *proc_t, PROCESS **p_ready)
{
	PROCESS	*p;
	int	greatest_ticks = 0;

	while ( 0 == greatest_ticks ) {
		for (p = proc_t; p < proc_t + NR_TASKS + NR_PROCS; 
			p++) {
			
			if (p->ticks > greatest_ticks) {
				greatest_ticks = p->ticks;
				(*p_ready) = p;
			}
		}

		if (greatest_ticks == 0) {
			for (p = proc_t; 
				p < proc_t + NR_TASKS + NR_PROCS; 
				p++) {
				
				p->ticks = p->priority / 5;
			}
		}
	}

}

