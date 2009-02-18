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


/*
; =========================================
; FLAGS - Intel 8086 Family Flags Register
; =========================================
;
;      |11|10|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|
;        |  | | | | | | | | | | | | | | | | '---  CF¡­¡­Carry Flag
;        |  | | | | | | | | | | | | | | | '---  1
;        |  | | | | | | | | | | | | | | '---  PF¡­¡­Parity Flag
;        |  | | | | | | | | | | | | | '---  0
;        |  | | | | | | | | | | | | '---  AF¡­¡­Auxiliary Flag
;        |  | | | | | | | | | | | '---  0
;        |  | | | | | | | | | | '---  ZF¡­¡­Zero Flag
;        |  | | | | | | | | | '---  SF¡­¡­Sign Flag
;        |  | | | | | | | | '---  TF¡­¡­Trap Flag  (Single Step)
;        |  | | | | | | | '---  IF¡­¡­Interrupt Flag
;        |  | | | | | | '---  DF¡­¡­Direction Flag
;        |  | | | | | '---  OF¡­¡­Overflow flag
;        |  | | | '-----  IOPL¡­¡­I/O Privilege Level  (286+ only)
;        |  | | '-----  NT¡­¡­Nested Task Flag  (286+ only)
;        |  | '-----  0
;        |  '-----  RF¡­¡­Resume Flag (386+ only)
;        '------  VM¡­¡­Virtual Mode Flag (386+ only)
;
;        ×¢: see   PUSHF  POPF  STI  CLI  STD  CLD
;
*/


/* Comments below accords to "Chapter 3.4.5: Segment Descriptors" of "Intel
      64 and IA-32 Arch. SW Developer’s Manual: Volume 3A: System Programming
      Guide". */
      
/* GDT Descriptor Attributes
        DA_ : Descriptor Attribute
        D    : Data Segment
        C    : Code Segment
        S    : System Segment
        R    : Readonly
        RW   : Read/Write
        A    : Access */
        
.set    DA_32, 0x4000 /* 32-bit segment */
.set	DA_LIMIT_4K, 0X8000

/* Descriptor privilege  level */
.set    DA_DPL0, 0x00    /* DPL = 0 */
.set    DA_DPL1, 0x20    /* DPL = 1 */
.set    DA_DPL2, 0x40    /* DPL = 2 */
.set    DA_DPL3, 0x60    /* DPL = 3 */

/* GDT Code- and Data-Segment Types */
.set    DA_DR,   0x90    /* Read-Only */
.set    DA_DRW,  0x92    /* Read/Write */
.set    DA_DRWA, 0x93    /* Read/Write, accessed */
.set    DA_C,    0x98    /* Execute-Only */
.set    DA_CR,   0x9A    /* Execute/Read */
.set    DA_CCO,  0x9C    /* Execute-Only, conforming */
.set    DA_CCOR, 0x9E    /* Execute/Read-Only, conforming */

/* GDT System-Segment and Gate-Descriptor Types */
.set    DA_LDT,      0x82    /* LDT */
.set    DA_TaskGate, 0x85     /* Task Gate */
.set    DA_386TSS,   0x89    /* 32-bit TSS(Available) */
.set    DA_386CGate, 0x8C     /* 32-bit Call Gate */
.set    DA_386IGate, 0x8E     /* 32-bit Interrupt Gate */
.set    DA_386TGate, 0x8F     /* 32-bit Trap Gate */

/* Selector Attributes */
/* RPL */
.set    SA_RPL0, 0
.set    SA_RPL1, 1
.set    SA_RPL2, 2
.set    SA_RPL3, 3

/* TI */
.set    SA_TIG,  0
.set    SA_TIL,  4

/* Paging Attributes */
.set	PG_P,	1	/* page exists */
.set	PG_RWR,	0	/* read and exec */
.set	PG_RWW,	2	/* r/w/x */
.set	PG_USS,	0	/* sys */
.set	PG_USU,	4	/* user */


/* MACROS */
/* Segment Descriptor data structure.
      Usage: Descriptor Base, Limit, Attr
       Base: 4byte
       Limit: 4byte (low 20 bits available)
       Attr: 2byte (lower 4 bits of higher byte are always 0) */

.macro Descriptor Base, Limit, Attr
	.2byte	\Limit & 0xFFFF
	.2byte	\Base  & 0xFFFF
	.byte	(\Base   >> 16) & 0xFF
	.2byte	((\Limit >> 8 ) & 0xF00) | (\Attr & 0xF0FF)
	.byte	(\Base   >> 24) & 0xFF
.endm

/* Gate
	usage: Gate Selector, Offset, DCount, Attr
		Selector:	2byte
		Offset:		4byte
		DCount:		byte
		Attr:		byte */
.macro Gate Selector, Offset, DCount, Attr
	.2byte	\Offset & 0xFFFF
	.2byte	\Selector
	.2yte	(\DCount   & 0x1f) | ((\Attr << 8) & 0xff00)
	.2byte	((\Offset >> 16 ) & 0xFFFF)
.endm

