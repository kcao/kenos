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

#ifndef	_KENOS_KLIBS_H_
#define	_KENOS_KLIBS_H_

#include "type.h"
#include "const.h"

/* klib.S */
PUBLIC void out_byte(t_port port, t_8 value);
PUBLIC t_8  in_byte(t_port port);
PUBLIC void disp_str(char * info);
PUBLIC void disp_color_str(char * info, int color);
PUBLIC void disable_int();
PUBLIC void enable_int();

#endif /* _KENOS_KLIBS_H_ */

