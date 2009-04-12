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

#ifndef _KENOS_ASSERT_H_
#define _KENOS_ASSERT_H_

#include "asm/system.h"
#include "klibs.h"
#include "klibc.h"

#define ASSERT(expr) \
    if (!(expr)) { \
        cli(); \
        disp_str("Assertion Failed in File "); \
        disp_str(__FILE__); \
        disp_str(" at line "); \
        disp_int(__LINE__); \
        disp_str("\n"); \
        hlt(); \
    }

#endif /* _KENOS_ASSERT_H_ */


