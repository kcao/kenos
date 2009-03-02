#****************************************************************************
# *   Copyright (C) 2009 by Cao, Chen                                       *
# *   ken.ccao@gmail.com                                                    *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# ***************************************************************************

CC		=	gcc
LD		=	ld
OBJCOPY		=	objcopy

CFLAGS		=	-I include -c
TRIM_FLAGS	=	-R .pdr -R .comment -R .note -S -O binary

LDFILE_BOOT	=	x86_boot.ld
LDFILE_LDR	=	x86_ldr.ld
LDFILE_KER	=	x86_ker.ld
LDFLAGS_BOOT	=	-s -T $(LDFILE_BOOT)
LDFLAGS_LDR	=	-s -T $(LDFILE_LDR)
LDFLAGS_KER	=	-s -e c -T $(LDFILE_KER)

all: boot.bin LOADER.BIN

boot.bin: boot.S
	$(CC) $(CFLAGS) boot.S
	$(LD) boot.o -o boot.elf $(LDFLAGS_BOOT)
	$(OBJCOPY) $(TRIM_FLAGS) boot.elf $@

LOADER.BIN: loader.S
	$(CC) $(CFLAGS) loader.S
	$(LD) loader.o -o loader.elf $(LDFLAGS_LDR)
	$(OBJCOPY) $(TRIM_FLAGS) loader.elf $@

boot.img: boot.bin
	dd if=boot.bin of=boot.img bs=512 count=1
	dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879

# run the os using bochs
run: bochsrc.bxrc boot.img
	bochs -f bochsrc.bxrc
	
# You must have the authority to do mount, or you must use "su root" 
#	or "sudo" command to do "make copy"
bcopy: boot.img LOADER.BIN
	mkdir -p tmp;\
	mount -o loop boot.img tmp/ -o fat=12;\
	cp LOADER.BIN tmp/;\
	umount tmp;\
	rm -rf tmp/;

clean: 
	rm -f *.o *.elf *.bin *.BIN bochsout.txt

dclean:
	rm -f *.img
