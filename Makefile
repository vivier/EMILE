#
#
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PACKAGE	= emile
VERSION	= 0.2CVS

# kernel arguments

# root filesystem on harddrive

KERNEL_ARGS="root=/dev/sda7"

# ramdisk

#KERNEL_ARGS = "root=/dev/ramdisk ramdisk_size=2048"

# NetBoot

#KERNEL_ARGS = "root=/dev/nfs ip=autoconf"

# ramdisk on 2nd floppy

#KERNEL_ARGS="vga=normal noinitrd load_ramdisk=1 prompt_ramdisk=1 ramdisk_size=16384 root=/dev/fd0 disksize=1.44 flavor=compact"

# tools to use

ifneq ($(shell uname -m),m68k)
CROSS_COMPILE	= m68k-linux-
endif

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy

# base address

BASE_ADDRESS	=	0x00200000

all: floppy.img

floppy.img: first/first vmlinuz second/second
	cat first/first > floppy.img.X
	cat second/second >> floppy.img.X
	mv floppy.img.X floppy.img

vmlinux.bin: vmlinux
	$(OBJCOPY) -O binary -R .note -R .comment -S vmlinux vmlinux.bin

vmlinuz: vmlinux.bin
	cp vmlinux.bin vmlinuz.out
	gzip -9 vmlinuz.out
	mv vmlinuz.out.gz vmlinuz

first/first::
	$(MAKE) -C first OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) \
		BASE_ADDRESS=$(BASE_ADDRESS)

second/second::
	$(MAKE) -C second OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) \
		BASE_ADDRESS=$(BASE_ADDRESS) KERNEL_ARGS=$(KERNEL_ARGS) \
		VERSION=$(VERSION)

dump: floppy.img
	dd if=floppy.img of=/dev/fd0 bs=512
	eject /dev/fd0

clean:
	$(MAKE) -C first clean
	$(MAKE) -C second clean
	rm -f floppy.img floppy.img.X vmlinuz vmlinux.bin

DISTFILES	= second/head.S second/MMU.c second/main.c \
		  second/MMU.h second/console.c second/Makefile \
		  second/console.h second/printf.c second/MMU_asm.S \
		  second/uncompress.h second/font_8x16.c \
		  second/ld.script second/memory.c second/inflate.c \
		  second/uncompress.c second/misc.c second/bootinfo.h \
		  second/misc.h second/lowmem.h second/bootinfo.c \
		  second/glue.h second/memory.h second/glue.S \
		  second/enter_kernel.S first/first.S first/Makefile \
		  Makefile COPYING README AUTHORS ChangeLog

dist:
	rm -fr $(PACKAGE)-$(VERSION)
	mkdir $(PACKAGE)-$(VERSION)
	for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(PACKAGE)-$(VERSION)/$$dir; \
		fi; \
		cp -p $$file $(PACKAGE)-$(VERSION)/$$file; \
	done
	rm -f $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION).tar.bz2
	tar cvf $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION)
	bzip2 -9 $(PACKAGE)-$(VERSION).tar
	rm -fr $(PACKAGE)-$(VERSION)
