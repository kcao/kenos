/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               sconst.inc
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
                               
/* sconst.h */
/***************************************************************************
 *   Modified by Cao, Chen - 2009                                          *
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


#ifndef	_KENOS_SCONST_H_
#define	_KENOS_SCONST_H_

.set P_STACKBASE,	0
.set GSREG,		P_STACKBASE
.set FSREG,		(GSREG		+ 4)
.set ESREG,		(FSREG		+ 4)
.set DSREG,		(ESREG		+ 4)
.set EDIREG,		(DSREG		+ 4)
.set ESIREG,		(EDIREG		+ 4)
.set EBPREG,		(ESIREG		+ 4)
.set KERNELESPREG,	(EBPREG		+ 4)
.set EBXREG,		(KERNELESPREG	+ 4)
.set EDXREG,		(EBXREG		+ 4)
.set ECXREG,		(EDXREG		+ 4)
.set EAXREG,		(ECXREG		+ 4)
.set RETADR,		(EAXREG		+ 4)
.set EIPREG,		(RETADR		+ 4)
.set CSREG,		(EIPREG		+ 4)
.set EFLAGSREG,		(CSREG		+ 4)
.set ESPREG,		(EFLAGSREG	+ 4)
.set SSREG,		(ESPREG		+ 4)
.set P_STACKTOP,	(SSREG		+ 4)
.set P_LDT_SEL,		P_STACKTOP
.set P_LDT,		(P_LDT_SEL	+ 4)

.set TSS3_S_SP0,	4

.set INT_M_CTL,		0x20	// I/O port for interrupt controller         <Master>
.set INT_M_CTLMASK,	0x21	// setting bits in this port disables ints   <Master>
.set INT_S_CTL,		0xA0	// I/O port for secondary interrupt controller  <Slave>
.set INT_S_CTLMASK,	0xA1	// setting bits in this port disables ints   <Slave>

.set EOI,	0x20	/* entry of interrupt */

// the value of the following vars must be the same with the ones in protect.h
.set SELECTOR_FLAT_C,		0x08		//
.set SELECTOR_TSS,		0x20		//
.set SELECTOR_KERNEL_CS,	SELECTOR_FLAT_C


#endif /* _KENOS_SCONST_H_ */

