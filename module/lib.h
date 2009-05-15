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
 
/* ==================================================================
   Routine: DispAL
   Action: Display the number in %al
*/
DispAL:
	push	%ecx
	push	%edx
	push	%edi
	
	mov	$0, %edi
	
	mov	$0x0f, %ah	/*0b0000: black bg, 0b1111: white fg*/
	mov	%al, %dl
	shr	$4, %al
	mov	$2, %ecx
.DAbegin:
	and	$0b1111, %al
	cmp	$9, %al
	ja	.DA1
	add	$'0', %al
	jmp	.DA2
.DA1:
	sub	$0x0a, %al
	add	$'A', %al
.DA2:
	mov	%ax, %gs:(%edi)
	add	$2, %edi
	
	mov	%dl, %al
	loop	.DAbegin
/*	add	$2, %edi	*/
	
//	mov	%edi, (dwDispPos)
	
	pop	%edi
	pop	%edx
	pop	%ecx
	
	ret
/* the end of DispAL */

