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

/* pm and kernel 
	boot.S
	loader.S
	kernel.S */

#include "pm.h"

.text
.code16

	jmp LABEL_START	/* Start to boot. */
	nop		/* nop required */

#include "addr.h"
#include "fat12hdr.h"
/* ==================================================================
   NOTE! Actually here we put the normal .data section into
   the .code section. For application SW, it is not allowed. However, we are
   writting an OS. That is OK. Because there is no OS to complain about
   that behavior. :) 
*/

/* Global Descriptor Table */
LABEL_GDT:          Descriptor 0,           0, 0

/* executable seg */
LABEL_DESC_FLAT_C:  Descriptor 0,       0xffff, DA_CR  | DA_32 | DA_LIMIT_4K

/* r/w seg */
LABEL_DESC_FLAT_RW: Descriptor 0,       0xffff, DA_DRW | DA_32 | DA_LIMIT_4K
LABEL_DESC_VIDEO:   Descriptor 0xB8000, 0xffff, DA_DRW | DA_DPL3

.set GdtLen, (. - LABEL_GDT)   /* GDT Length */
GdtPtr:	.2byte   (GdtLen - 1)  /* GDT Limit */
	.4byte   (BaseOfLoaderPhyAddr + LABEL_GDT)  /* GDT Base */

/* GDT Selector */
.set SelectorFlatC,  (LABEL_DESC_FLAT_C - LABEL_GDT)
.set SelectorFlatRW, (LABEL_DESC_FLAT_RW - LABEL_GDT)
.set SelectorVideo,  (LABEL_DESC_VIDEO - LABEL_GDT + SA_RPL3)

.set BaseOfStack, 0x0100

.set MixJOffset, (BaseOfLoaderPhyAddr + LABEL_PM_START)

/*==================================================================
  ==================================================================
  Program starts here.  
*/
LABEL_START:
	mov	 %cs,%ax
	mov	 %ax,%ds
	mov	 %ax,%es
	mov	 %ax,%ss
	mov	 $BaseOfStack, %sp
	
	/* Display "Loading^^" */
	mov	 $0,%dh
	call	DispStrRealMode	/* Display string(index 0)*/
	

/* ==================================================================
	Get the size of memory
*/
	movl	$0, %ebx	
	mov	$_MemChkBuf, %di
.MemChkLoop:
	mov	$0xe820, %eax
	mov	$20, %ecx
	mov	$0x534D4150, %edx
	int	$0x15
	jc	.MemChkFail
	
	add	$20, %di
	incl	(_dwMCRNumber)
	cmp	$0, %ebx
	jne	.MemChkLoop
	jmp	.MemChkOK
.MemChkFail:
	movl	$0, (_dwMCRNumber)
.MemChkOK:

/* ==================================================================
	Find KERNEL.BIN in root directory of driver 0 
*/
	movw	$SecNoOfRootDir, (wSectorNo)

	/* Reset floppy */
	xor	 %ah,%ah
	xor	 %dl,%dl       /* %dl=0: floppy driver 0 */
	int	 $0x13         /* BIOS int 13h, ah=0: Reset driver 0 */
	
/* Read root dir sector to memory */
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:	
	cmpw	$0,(wRootDirSizeForLoop)    /* If searching in root dir */
	jz	LABEL_NO_KERNELBIN          /* can find  KERNEL.BIN ?   */
	decw	(wRootDirSizeForLoop)
	mov	$BaseOfKernelFile,%ax
	mov	%ax,%es                     /* %es <- BaseOfKernelFile*/
	mov	$OffsetOfKernelFile,%bx     /* %bx <- OffsetOfKernelFile */
	mov	(wSectorNo),%ax             /* %ax <- sector number in root */
	mov	$1,%cl
	call	ReadSector
	
	mov     $KernelFileName,%si         /* %ds:%si -> KERNERL  BIN */
	mov     $OffsetOfKernelFile,%di     /* %es:%di -> BaseOfKernelFile */
	cld
	mov     $0x10,%dx

/* Search for "KERNEL  BIN", FAT12 save file name in 12 bytes, 8 bytes for 
   file name, 3 bytes for suffix, last 1 bytes for '\20'. If file name is
   less than 8 bytes, filled with '\20'. So "LOADER.BIN" is saved as:
   "LOADER  BIN"(4f4c 4441 5245 2020 4942 204e --> OL DA RE __ IB _N)
*/
LABEL_SEARCH_FOR_KERNELBIN:
	cmp	$0,%dx                      /* Read control */
	jz      LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR
	dec     %dx
	mov     $11,%cx

LABEL_CMP_FILENAME:
	cmp	$0,%cx
	jz      LABEL_FILENAME_FOUND    /* If 11 chars are all identical? */
	dec     %cx
	lodsb                           /* %ds:(%si) -> %al*/
	cmp     %es:(%di),%al
	jz      LABEL_GO_ON
	jmp     LABEL_DIFFERENT         /* Different */

LABEL_GO_ON:
	inc	%di
	jmp     LABEL_CMP_FILENAME      /* Go on loop */

LABEL_DIFFERENT:
	and     $0xffe0,%di             /* Go to head of this entry */
	add     $0x20,%di
	mov     $KernelFileName,%si     /* Next entry */
	jmp     LABEL_SEARCH_FOR_KERNELBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	addw    $1,(wSectorNo)
	jmp     LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_KERNELBIN:
	mov	$2,%dh
	call    DispStrRealMode         /* Display string(index 2) */
	jmp        .            /* Infinite loop */

LABEL_FILENAME_FOUND:
	mov	$RootDirSectors,%ax
	and     $0xfff0,%di     /* Start of current entry, 32 bytes per entry */

	push	%eax
	mov	%es:0x1c(%di), %eax
	movl	%eax, (dwKernelSize)
	pop	%eax

	add     $0x1a,%di               /* First sector of this file */
	mov     %es:(%di),%cx
	push    %cx                     /* Save index of this sector in FAT */
	add     %ax,%cx
	add     $DeltaSecNo,%cx         /* kernel.bin's start sector saved in %cl */
	mov     $BaseOfKernelFile,%ax
	mov     %ax,%es                 /* %es <- BaseOfKernelFile */
	mov     $OffsetOfKernelFile,%bx /* %bx <- OffsetOfKernelFile */
	mov     %cx,%ax                 /* %ax <- Sector number */

LABEL_GOON_LOADING_FILE:
	push	%ax
	push    %bx
	mov     $0x0e,%ah
	mov     $'.',%al    /* Char to print */
	mov     $0x0f,%bl   /* Front color: white */
	int     $0x10       /* BIOS int 10h, ah=0xe: Print char */
	pop     %bx
	pop     %ax

	mov     $1,%cl
	call    ReadSector
	pop     %ax             /* Got index of this sector in FAT */
	call    GetFATEntry
	cmp     $0x0fff,%ax
	jz      LABEL_FILE_LOADED
	push    %ax             /* Save index of this sector in FAT */
	mov     $RootDirSectors,%dx
	add     %dx,%ax
	add     $DeltaSecNo,%ax
	add     (BPB_BytsPerSec),%bx
	jmp     LABEL_GOON_LOADING_FILE

LABEL_FILE_LOADED:
	call	KillMotor
	
	mov	$1,%dh		/*"K Ready. " */
	call	DispStrRealMode /* Display string(index 1) */
	nop

	
/* ==================================================================
	prepare to enter pm.
*/
	/* Load GDTR(Global Descriptor Table Register) */
	lgdtw   GdtPtr

	/* Clear Interrupt Flags */
	cli

	/* Open A20 line. */
	inb	 $0x92, %al
	orb	 $0b00000010, %al
	outb	%al, $0x92
	
	/* Enable protect mode, PE bit of CR0. */
	movl	%cr0, %eax
	orl	 $1, %eax
	movl	%eax, %cr0

	/* Mixed-Size Jump. */
	.2byte 0xea66		/* GAS can not handle mixed-size jump. */
	.4byte MixJOffset	/* We have to code it manually. It is */
				/* an ugly  way.*/
	.2byte SelectorFlatC	/* = jmp dword SelectorFlatC:0 (NASM) */

	jmp	.		/* seems never to be executed */


/* ==================================================================
   ==================================================================
   Variable table
*/
wRootDirSizeForLoop:	.2byte  RootDirSectors
wSectorNo:		.2byte  0       /* Sector number to read */
bOdd:			.byte   0       /* is odd? */
dwKernelSize:		.4byte	0

/* ==================================================================
   String table
*/
KernelFileName:		.asciz	"KERNEL  BIN"      /* File name */
.set		MessageLength,9
LoadMessage:		.ascii	"Loading^^"        /* index 0 */
Message1:		.ascii	"K Ready. "        /* index 1 */
Message2:		.ascii	"No KERNEL"        /* index 2 */

/* ==================================================================
   Routine: DispStrRealMode
   Action: Display a string, string index stored in %dh
*/
DispStrRealMode: 
	mov     $MessageLength, %ax
	mul     %dh
	add     $LoadMessage,%ax
	mov     %ax,%bp               /* String address */
	mov     %ds,%ax
	mov     %ax,%es
	mov     $MessageLength,%cx    /* String length */
	mov     $0x1301,%ax           /* ah = 0x13, al = 0x01(W) */
	mov     $0x07,%bx             /* PageNum 0(bh = 0), bw(bl= 0x07)*/
	mov     $0,%dl                /* Start row and column */
	add	$3,%dh
	int     $0x10                 /* BIOS INT 10h, display string */
	ret

/* ==================================================================
   Routine: ReadSector
   Action: Read %cl Sectors from %ax sector(floppy) to %es:%bx(memory) 
	 Assume sector number is 'x', then:
	   x/(BPB_SecPerTrk) = y,
	   x%(BPB_SecPerTrk) = z.
	 The remainder 'z' PLUS 1 is the start sector number;
	 The quotient 'y' devide by BPB_NumHeads(RIGHT SHIFT 1 bit)is cylinder
	   number;
	 AND 'y' by 1 can get magnetic header.
*/
ReadSector:
	push    %ebp
	mov     %esp,%ebp
	sub     $2,%esp        /* Reserve space for saving %cl */
	
	mov     %cl,-2(%ebp)
	push    %bx            /* Save bx */
	mov     (BPB_SecPerTrk), %bl    /* %bl: the devider */
	div     %bl            /* 'y' in %al, 'z' in %ah */
	inc     %ah            /* z++, got start sector */
	mov     %ah,%cl        /* %cl <- start sector number */
	mov     %al,%dh        /* %dh <- 'y' */
	shr     $1,%al         /* 'y'/BPB_NumHeads */
	mov     %al,%ch        /* %ch <- Cylinder number(y>>1) */
	and     $1,%dh         /* %dh <- Magnetic header(y&1) */
	pop     %bx            /* Restore %bx */
	/* Now, we got cylinder number in %ch, start sector number in %cl,
	   magnetic header in %dh. */
	mov     (BS_DrvNum), %dl
GoOnReading:
	mov     $2,%ah
	mov     -2(%ebp),%al    /* Read %al sectors */
	int     $0x13
	jc      GoOnReading     /* If CF set 1, mean read error, reread. */
	
	add     $2,%esp
	pop     %ebp
	ret

/* ==================================================================
   Routine: GetFATEntry
   Action: Find %ax sector's index in FAT, save result in %ax 
*/
GetFATEntry:
	push    %es
	push    %bx
	push    %ax
	mov     $BaseOfKernelFile,%ax
	sub     $0x0100,%ax
	mov     %ax,%es           /* Left 4K bytes for FAT */
	pop     %ax
	movb    $0,(bOdd)
	mov     $3,%bx
	mul     %bx               /* %dx:%ax = %ax*3 */
	mov     $2,%bx
	div     %bx               /* %dx:%ax/2 */
	cmp     $0,%dx            /* remainder %dx = 0 ? */
	jz      LABEL_EVEN
	movb    $1,(bOdd)

LABEL_EVEN:
	xor     %dx,%dx           /* Now %ax is the offset of FATEntry in FAT */
	mov     (BPB_BytsPerSec),%bx
	div     %bx               /* %dx:%ax/BPB_BytsPerSec */
	push    %dx
	mov     $0,%bx
	add     $SecNoOfFAT1,%ax  /* %ax <- FATEntry's sector */
	mov     $2,%cl            /* Read 2 sectors in 1 time, because FATEntry */
	call    ReadSector        /* may be in 2 sectors. */
	pop     %dx
	add     %dx,%bx
	mov     %es:(%bx),%ax
	cmpb    $1,(bOdd)
	jnz     LABEL_EVEN_2
	shr     $4,%ax

LABEL_EVEN_2:
	and     $0x0fff,%ax

LABEL_GET_FAT_ENTRY_OK:
	pop     %bx
	pop     %es
	ret

/* ==================================================================
   Routine: KillMotor
   Action: turn off the motor
*/
KillMotor:
	push	%dx
	mov	$0x03f2, %dx
	mov	$0, %al
	outb	%al, %dx
	pop	%dx
	ret


/* ==================================================================
   ==================================================================
   from here on, the codes are executed under pm.
   32-bit code segment, jump from rm.
   ==================================================================
   ==================================================================
*/
LABEL_PM_START:
.code32
	mov	$(SelectorVideo), %ax
	mov	%ax, %gs              /* Video segment selector(dest) */
	mov	$(SelectorFlatRW), %ax
	mov	%ax, %ds              /* r/w segment selector(dest) */
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %ss
	mov	$TopOfStack, %esp
	
	push	$szMemChkTitle
	call	DispStr
	add	$4, %esp
	
	call	DispMemInfo
	call	SetupPaging

	movl	$((80 * 0 + 38) * 2), %edi	/* print at line 0, col 20 */
	movb	$0xC, %ah               /* 0000: Black Back 1100: Red Front */
	movb	$'P', %al
	mov	 %ax, %gs:(%edi)
	
	
	call	InitKernel
	
	jmp	$SelectorFlatC, $KernelEntryPointPhyAddr
	
	/* Stop here, infinate loop. */
/*	jmp	 .	*/


#include "lib.h"

/* dispaly the info of memory */
DispMemInfo:
	pushl	%esi
	pushl	%edi
	pushl	%ecx
	
	movl	$MemChkBuf, %esi    /* %esi contains the addr of MemChkBuf */
	movl	(dwMCRNumber), %ecx /* %ecx contains the value of dwMCRNumber */

.DMloop:
	movl	$5, %edx
	movl	$ARDStruct, %edi
.DM1:
	pushl	(%esi)
	call	DispInt
	
	popl	%eax
	stosl
	addl	$4, %esi
	decl	%edx
	cmpl	$0, %edx
	jnz	.DM1
	call	DispReturn
	
	cmpl	$1, (dwType)
	jne	.DM2
	mov	(dwBaseAddrLow), %eax
	add	(dwLengthLow), %eax
	cmpl	(dwMemSize), %eax
	jb	.DM2
	movl	%eax, (dwMemSize)
.DM2:
	loop	.DMloop
	
	call	DispReturn
	push	$szRAMSize
	call	DispStr
	add	$4, %esp

	pushl	dwMemSize
	call	DispInt
	add	$4, %esp
/* -------------------------------- */
	pushl	%eax
	push	%es
	pushl	%edi
	
	movl	$0x0000, %eax
	mov	%ax, %es
	mov	$0x7c00, %edi /* refer to doc/kenos/mem_map */
	
	mov	(dwMemSize), %eax
	movl	%eax, (%edi)
	
	popl	%edi
	pop	%es
	popl	%eax
/* -------------------------------- */
	pop	%ecx
	pop	%edi
	pop	%esi

	ret
/* done, dispaly the info of memory */

/* setup paging */
SetupPaging:
	/* decide the number of PDE and pts to be initialized, 
	 * based on the size of mem */
	xor	%edx, %edx
	mov	(dwMemSize), %eax
	mov	$0x00400000, %ebx	/* the mem size of one page table */

	div	%ebx
	mov	%eax, %ecx	/* %ecx contains the num of pts, 
				 * i.e. num of PDE */
	test	%edx, %edx
	jz	.no_remainder
	inc	%ecx
.no_remainder:
	push	%ecx	/* save the num of PT */
	
	/* initialize the page directory */
	mov	$(SelectorFlatRW), %ax
	mov	%ax, %es
	mov	$(PageDirBase), %edi
	xor	%eax, %eax
	mov	$(PageTblBase | PG_P | PG_USU | PG_RWW), %eax
.SP1:
	stosl
	add	$4096, %eax
	loop	.SP1
	
	/* initialize the page tables */
	pop	%eax	/* the num of PTs */
	mov	$1024, %ebx	/* assign 1024 PTEs to each PT */
	mul	%ebx
	mov	%eax, %ecx	/* now %ecx contains the num of PTEs */
	
	/* the header addr of this seg is PageTblBase */
	mov	$(PageTblBase), %edi	
	xor	%eax, %eax
	mov	$(PG_P  | PG_USU | PG_RWW), %eax
.SP2:
	stosl
	add	$4096, %eax
	loop	.SP2

	xorl	%eax, %eax	/* %eax is still not 0x0?! */
	movl	$(PageDirBase), %eax
	movl	%eax, %cr3
	xorl	%ebx, %eax
	movl	%cr0, %eax
	orl	$0x80000000, %eax
	movl	%eax, %cr0
	
	jmp	.SP3
.SP3:
	nop
	
	ret
/* finished setting up paging*/

/* InitKernel
	arrange the kernel in the mem.
	traverse every Program Header, 
	and place the kernel properly according to the info in Program Header */
InitKernel:
	xor	%esi, %esi
	
	/* ecx = pELFHdr->e_phnum */
	movw	(BaseOfKernelFilePhyAddr + 0x2c), %cx
	movzx	%cx, %ecx
	
	/* esi = pELFHdr->e_phoff */
	mov	(BaseOfKernelFilePhyAddr + 0x1c), %esi
	add	$BaseOfKernelFilePhyAddr, %esi

/*
 * for (i = 0; i < %ecx; i++) {
 * 	if (p_type == 0) {
 *		continue;
 *		
 *	}
 *	
 *	memcpy(dst, src, size);
 *	s = s->next;	// s += e_phentsize
 * }
 *
 **/
.IKBegin:
	mov	0(%esi), %eax
	cmp	$0, %eax
	jz	.NoAction
	
	pushl	16(%esi)
	mov	4(%esi), %eax
	add	$BaseOfKernelFilePhyAddr, %eax
	push	%eax
	pushl	8(%esi)
	
	call	MemCpy
	add	$12, %esp
.NoAction:
	add	$0x20, %esi
	dec	%ecx
	
	jnz	.IKBegin

	ret
/* end of InitKernel */

	
/* the data definition goes here */
LABEL_DATA:

/* data used under RM */
/* strings */
_szMemChkTitle:	.asciz "BaseAddrL BaseAddrH LengthLow LengthHigh Type\n"
_szRAMSize:	.asciz "RAM size:"
_szReturn:	.asciz "\n"
/* variables */
_dwMCRNumber:	.4byte	0	/* Memory Check Result */
_dwDispPos:	.4byte	(80 * 6 + 0) * 2	/* line 6, col 0 */
_dwMemSize:	.4byte	0
_ARDStruct:	/* Address Range Descriptor Structure */
	_dwBaseAddrLow: 	.4byte 0
	_dwBaseAddrHigh:	.4byte 0
	_dwLengthLow:		.4byte 0
	_dwLengthHigh:		.4byte 0
	_dwType:		.4byte 0
_MemChkBuf:
	.rept   256
	.byte   0
	.endr

/* data used under PM */
.set szMemChkTitle,	(BaseOfLoaderPhyAddr + _szMemChkTitle)
.set szRAMSize,		(BaseOfLoaderPhyAddr + _szRAMSize)
.set szReturn,		(BaseOfLoaderPhyAddr + _szReturn)
.set dwDispPos,		(BaseOfLoaderPhyAddr + _dwDispPos)
.set dwMemSize,		(BaseOfLoaderPhyAddr + _dwMemSize)
.set dwMCRNumber,	(BaseOfLoaderPhyAddr + _dwMCRNumber)
.set ARDStruct,		(BaseOfLoaderPhyAddr + _ARDStruct)
.set 	dwBaseAddrLow,	(BaseOfLoaderPhyAddr + _dwBaseAddrLow)
.set 	dwBaseAddrHigh,	(BaseOfLoaderPhyAddr + _dwBaseAddrHigh)
.set 	dwLengthLow,	(BaseOfLoaderPhyAddr + _dwLengthLow)
.set 	dwLengthHigh,	(BaseOfLoaderPhyAddr + _dwLengthHigh)
.set 	dwType,		(BaseOfLoaderPhyAddr + _dwType)
.set MemChkBuf,		(BaseOfLoaderPhyAddr + _MemChkBuf)


/* the stack is right at the end of the data section*/

StackSpace:
	.rept   0x1000
	.byte   0
	.endr
	
.set TopOfStack,	(BaseOfLoaderPhyAddr + .)	/* the top of stack */

/* end of data section ------------ */

