/***************************************************************************
 *   Copyright (C) 2009 by Open source contributors and Cao, Chen          *
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

#ifndef _KENOS_IO_H
#define _KENOS_IO_H


/* copied from linux v0.11
 * include/asm/io.h 
 */
#define outb(value, port) \
__asm__ ("outb %%al, %%dx"::"a" (value),"d" (port))


#define inb(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx, %%al":"=a" (_v):"d" (port)); \
_v; \
})


/* 'outb' and 'inb' with pause */
#define outb_p(value, port) \
__asm__ ("outb %%al,%%dx\n" \
                "\tjmp 1f\n" \
                "1:\tjmp 1f\n" \
                "1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
        "\tjmp 1f\n" \
        "1:\tjmp 1f\n" \
        "1:":"=a" (_v):"d" (port)); \
_v; \
})
/* end of "copy from linux v0.11 */

#define outw(value, port) \
__asm__ ("outw %%ax, %%dx"::"a" (value),"d" (port))


#define inw(port) ({ \
unsigned short _v; \
__asm__ volatile ("inw %%dx, %%ax":"=a" (_v):"d" (port)); \
_v; \
})


#endif /* _KENOS_IO_H */

