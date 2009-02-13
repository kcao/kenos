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


/* real mode loader, null kernel */
.code16
.set    BaseOfStack,        0x0100    /* Stack base address, inner */

.set	BaseOfKernelFile,   0x8000
.set	OffsetOfKernelFile, 0x0000

.text
    
    /* Floppy header of FAT12 */
    jmp     LABEL_START /* Start to boot. */
    nop                 /* nop required */

#include "fat12hdr.h" 

LABEL_START:
    mov     %cs, %ax
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %ss
    mov     $BaseOfStack, %sp
    
    /* Display "Loading^^" */
    mov     $0,%dh
    call    DispStr		/* Display string(index 0)*/

    /* Find KERNEL.BIN in root directory of driver 0 */
    movw    $SecNoOfRootDir, (wSectorNo)

    /* Reset floppy */
    xor     %ah, %ah
    xor     %dl, %dl       /* %dl=0: floppy driver 0 */
    int     $0x13         /* BIOS int 13h, ah=0: Reset driver 0 */
    
/* Read root dir sector to memory */
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:	
    cmpw    $0, (wRootDirSizeForLoop)    /* If searching in root dir */
    jz      LABEL_NO_KERNELBIN          /* can find  KERNEL.BIN ?   */
    decw    (wRootDirSizeForLoop)
    mov     $BaseOfKernelFile,%ax
    mov     %ax,%es                     /* %es <- BaseOfKernelFile*/
    mov     $OffsetOfKernelFile,%bx     /* %bx <- OffsetOfKernelFile */
    mov     (wSectorNo),%ax             /* %ax <- sector number in root */
    mov     $1,%cl
    call    ReadSector
    
    mov     $KernelFileName, %si         /* %ds:%si -> KERNERL  BIN */
    mov     $OffsetOfKernelFile, %di     /* %es:%di -> BaseOfKernelFile */
    cld
    mov     $0x10,%dx

/* Search for "KERNEL  BIN", FAT12 save file name in 12 bytes, 8 bytes for 
   file name, 3 bytes for suffix, last 1 bytes for '\20'. If file name is
   less than 8 bytes, filled with '\20'. So "LOADER.BIN" is saved as:
   "LOADER  BIN"(4f4c 4441 5245 2020 4942 204e --> OL DA RE __ IB _N)
*/
LABEL_SEARCH_FOR_KERNELBIN:
    cmp     $0,%dx                      /* Read control */
    jz      LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR
    dec     %dx
    mov     $11,%cx

LABEL_CMP_FILENAME:
    cmp     $0,%cx
    jz      LABEL_FILENAME_FOUND    /* If 11 chars are all identical? */
    dec     %cx
    lodsb                           /* %ds:(%si) -> %al*/
    cmp     %es:(%di),%al
    jz      LABEL_GO_ON
    jmp     LABEL_DIFFERENT         /* Different */

LABEL_GO_ON:
    inc     %di
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
    mov     $2,%dh
    call    DispStr         /* Display string(index 2) */
    jmp     .            /* Infinite loop */

LABEL_FILENAME_FOUND:
    mov     $RootDirSectors,%ax
    and     $0xfff0,%di     /* Start of current entry, 32 bytes per entry */

    push	%eax
    mov		%es:0x1c(%di), %eax
    movl	%eax, (dwKernelSize)
    pop		%eax

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
    push    %ax
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
    
    mov     $1,%dh			/*"K Ready. " */
    call    DispStr        /* Display string(index 1) */
    
    jmp	.		/* Infinite loop */
/* ==================================================================
   Variable table
*/
wRootDirSizeForLoop:    .2byte  RootDirSectors
wSectorNo:              .2byte  0       /* Sector number to read */
bOdd:                   .byte   0       /* odd or even? */
dwKernelSize:		.4byte	0

/* ==================================================================
   String table
*/
KernelFileName:     .asciz  "KERNEL  BIN"        /* File name */
.set            MessageLength,9
LoadMessage:        .ascii    "Loading^^"        /* index 0 */
Message1:           .ascii    "K Ready. "        /* index 1 */
Message2:           .ascii    "No KERNEL"        /* index 2 */

/* ==================================================================
   Routine: DispStr
   Action: Display a string, string index stored in %dh
*/
DispStr: 
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
    add		$3,%dh
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
    /* Now, we got cylinder number in %ch, start sector number in %cl, magnetic
     * header in %dh. */
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
