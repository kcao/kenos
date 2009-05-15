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

CFLAGS		=	-I ../include -c -fno-builtin -fno-stack-protector

LDFILE_MOD	=	./x86_mod.ld
LDFLAGS_MOD	=	-s -T $(LDFILE_MOD)

MODFILE		=	XMOD.ELF

MODOBJS		=	xmodint.o \
			nullxmod.o xsched01.o

.PHONY: all

all: $(MODFILE)

xmodint.o: xmodint.S
	$(CC) $(CFLAGS) $< -o $@

nullxmod.o: nullxmod.c
	$(CC) $(CFLAGS) $< -o $@

xsched01.o: xsched01.c
	$(CC) $(CFLAGS) $< -o $@

$(MODFILE): $(MODOBJS)
	$(LD) $(LDFLAGS_MOD) $(MODOBJS) -o $@

cp: $(MODFILE) /home/marc/vm/kenhdvfat12.img
	mkdir -p tmp;\
	mount -o loop /home/marc/vm/kenhdvfat12.img tmp/;\
	cp -f $(MODFILE) tmp/;\
	umount tmp/;\
	rm -rf tmp
	
rm: $(MODFILE) /home/marc/vm/kenhdvfat12.img
	mkdir -p tmp;\
	mount -o loop /home/marc/vm/kenhdvfat12.img tmp/;\
	rm -f tmp/$(MODFILE);\
	umount tmp/;\
	rm -rf tmp

dcl:
	rm -rf $(MODFILE) $(MODOBJS)

