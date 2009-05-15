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

#ifndef	_KENOS_MOD_CONST_H_
#define	_KENOS_MOD_CONST_H_

#define	BaseOfModFilePhyAddr	0x7e00
#define	XModPhyAddr		0x30400

/* elf header */
#define	ELFphoffOFFSET		0x1c
#define	ELFphentsizeOFFSET	0x2a
#define	ELFphnumOFFSET		0x2c

#define	ELFphoffLEN		4
#define	ELFphentsizeLEN		2
#define	ELFphnumLEN		2

/* program header */
#define	PHtypeOFFSET		0x0
#define	PHoffsetOFFSET		0x4
#define	PHvaddrOFFSET		0x8
#define	PHpaddrOFFSET		0xc
#define	PHfileszOFFSET		0x10

#define	PHtypeLEN		4
#define	PHoffsetLEN		4
#define	PHvaddrLEN		4
#define	PHpaddrLEN		4
#define	PHfileszLEN		4

#endif /* _KENOS_MOD_CONST_H_ */


