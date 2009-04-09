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

BFLAGS		=	-I boot/include -I include/fs -c
CFLAGS		=	-I include -c -fno-builtin -fno-stack-protector
TRIM_FLAGS	=	-R .pdr -R .comment -R .note -S -O binary

LDFILE_BOOT	=	boot/x86_boot.ld
LDFILE_LDR	=	boot/x86_ldr.ld
LDFILE_KER	=	kernel/x86_ker.ld

LDFLAGS_BOOT	=	-s -T $(LDFILE_BOOT)
LDFLAGS_LDR	=	-s -T $(LDFILE_LDR)
LDFLAGS_KER	=	-s -e c -T $(LDFILE_KER)

BINCLUDE	=	boot/include/addr.h include/fs/fat12hdr.h \
			boot/include/lib.h boot/include/pm.h

BOOTFILE	=	boot/boot.bin boot/LOADER.BIN
KERNELFILE	=	KERNEL.BIN
OBJS		=	kernel/kernel.o kernel/start.o kernel/i8259.o \
			kernel/protect.o \
			kernel/global.o kernel/main.o \
			kernel/clock.o \
			kernel/syscall.o kernel/proc.o \
			kernel/keyboard.o \
			kernel/tty.o kernel/console.o \
			kernel/printf.o kernel/vsprintf.o \
			kernel/panic.o \
			kernel/xsched.o \
			driver/ide.o driver/hd.o \
			module/manmod.o \
			fs/sfile.o \
			lib/klib.o lib/klibc.o lib/string.o 
			
			
BOBJS		=	boot/boot.o boot/boot.elf boot/loader.o boot/loader.elf

.PHONY: all clean dclean

all: boot.img $(BOOTFILE) $(KERNELFILE)

#
boot/boot.bin: boot/boot.S $(BINCLUDE)
	$(CC) $(BFLAGS) $< -o boot/boot.o
	$(LD) $(LDFLAGS_BOOT) boot/boot.o -o boot/boot.elf
	$(OBJCOPY) $(TRIM_FLAGS) boot/boot.elf $@

boot/LOADER.BIN: boot/loader.S $(BINCLUDE)
	$(CC) $(BFLAGS) $< -o boot/loader.o
	$(LD) $(LDFLAGS_LDR) boot/loader.o -o boot/loader.elf
	$(OBJCOPY) $(TRIM_FLAGS) boot/loader.elf $@

#
lib/string.o: lib/string.S
	$(CC) $(CFLAGS) $< -o $@
	
lib/klib.o: lib/klib.S
	$(CC) $(CFLAGS) $< -o $@

lib/klibc.o:lib/klib.c
	$(CC) $(CFLAGS) $< -o $@

#
driver/ide.o: driver/ide.c
	$(CC) $(CFLAGS) $< -o $@

driver/hd.o: driver/hd.c
	$(CC) $(CFLAGS) $< -o $@

#
kernel/kernel.o: kernel/kernel.S
#	nasm -f elf -o kernel/kernel.o kernel/kernel.asm
	$(CC) $(CFLAGS) $< -o $@

kernel/start.o: kernel/start.c
	$(CC) $(CFLAGS) $< -o $@

kernel/i8259.o: kernel/i8259.c
	$(CC) $(CFLAGS) $< -o $@

kernel/protect.o: kernel/protect.c
	$(CC) $(CFLAGS) $< -o $@

kernel/global.o: kernel/global.c
	$(CC) $(CFLAGS) $< -o $@
	
kernel/main.o: kernel/main.c
	$(CC) $(CFLAGS) $< -o $@

kernel/clock.o: kernel/clock.c
	$(CC) $(CFLAGS) $< -o $@

kernel/syscall.o: kernel/syscall.S
	$(CC) $(CFLAGS) $< -o $@

kernel/proc.o: kernel/proc.c
	$(CC) $(CFLAGS) $< -o $@

kernel/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) $< -o $@

kernel/tty.o: kernel/tty.c
	$(CC) $(CFLAGS) $< -o $@

kernel/console.o: kernel/console.c
	$(CC) $(CFLAGS) $< -o $@

kernel/printf.o: kernel/printf.c
	$(CC) $(CFLAGS) $< -o $@

kernel/vsprintf.o: kernel/vsprintf.c
	$(CC) $(CFLAGS) $< -o $@

kernel/panic.o: kernel/panic.c
	$(CC) $(CFLAGS) $< -o $@

kernel/xsched.o: kernel/xsched.c
	$(CC) $(CFLAGS) $< -o $@

module/manmod.o: module/manmod.c
	$(CC) $(CFLAGS) $< -o $@

fs/sfile.o: fs/sfile.S
	$(CC) $(CFLAGS) $< -o $@

$(KERNELFILE): $(OBJS)
	$(LD) $(LDFLAGS_KER) $(OBJS) -o $@
#	$(LD) -s -Ttext 0x30400 $(OBJS) -o $@


boot.img: boot/boot.bin
	dd if=boot/boot.bin of=boot.img bs=512 count=1
	dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879

# run the os using bochs
run: bochsrc.bxrc boot.img
	bochs -f bochsrc.bxrc

# You must have the authority to do mount, or you must use "su root" 
#	or "sudo" command to do "make copy"
cp: boot.img boot/LOADER.BIN KERNEL.BIN
	mkdir -p tmp;\
	mount -o loop boot.img tmp/ -o fat=12;\
	cp boot/LOADER.BIN tmp/;\
	cp boot/boot.bin tmp/;\
	cp KERNEL.BIN tmp/;\
	umount tmp;\
	rm -rf tmp;

cl: 
	rm -f $(OBJS) $(BOBJS)

dcl:
	rm -f $(BOBJS) $(OBJS) $(BOOTFILE) $(KERNELFILE) boot.img bochsout.txt

