#
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PACKAGE	= emile
VERSION	= 0.5CVS

# kernel boot arguments

RAMDISK=$(shell ls ramdisk.gz 2> /dev/null)

ifeq ($(RAMDISK),ramdisk.gz)
KERNEL_ARGS="root=/dev/ramdisk ramdisk_size=2048"
else
# NFS boot
#KERNEL_ARGS="root=/dev/nfs ip=dhcp nfsroot=192.168.100.1:/nfsroot rw"
# SCSI boot
KERNEL_ARGS="root=/dev/sda2"
endif

# build info

WHO	= $(shell whoami)
WHERE	= $(shell hostname)
WHEN	= $(shell LANG=C date)
ARCH	= $(shell uname -m -o)

SIGNATURE = $(PACKAGE)-$(VERSION) $(WHO)@$(WHERE)($(ARCH)) $(WHEN)

# tools to use

ifneq ($(shell uname -m),m68k)
CROSS_COMPILE	= m68k-linux-
endif

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy

# identify architecture of kernel (Motorola 680x0 or PowerPC)

KERNEL=vmlinux
FILE=file -bknL
KERNEL_SIZE=$(shell ls -l vmlinux.bin | awk '{print $$5}')

all: floppy.img

floppy.img: tools first/first vmlinuz second/second $(RAMDISK)
ifeq ($(RAMDISK),ramdisk.gz)
	tools/emile-install -f first/first -s second/second \
			    -i vmlinuz -b $(KERNEL_SIZE) \
			    -r $(RAMDISK) \
			     floppy.img.X
else
	tools/emile-install -f first/first -s second/second \
			    -i vmlinuz -b $(KERNEL_SIZE) \
			     floppy.img.X
endif
	tools/emile-set-cmdline floppy.img.X $(KERNEL_ARGS)
	mv floppy.img.X floppy.img

vmlinux.bin: $(KERNEL)
	$(OBJCOPY) -I elf32-big -O binary -R .note -R .comment -S $(KERNEL) vmlinux.bin

vmlinuz: vmlinux.bin
	cp vmlinux.bin vmlinuz.out
	gzip -9 vmlinuz.out
	mv vmlinuz.out.gz vmlinuz

first/first::
	$(MAKE) -C first OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) SIGNATURE="$(SIGNATURE)"

second/second::
	$(MAKE) -C second OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) \
		VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)"

tools::
	$(MAKE) -C tools all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)"


dump: floppy.img
	dd if=floppy.img of=/dev/fd0 bs=512
	eject /dev/fd0

clean:
	$(MAKE) -C tools clean
	$(MAKE) -C first clean
	$(MAKE) -C second clean
	rm -f floppy.img floppy.img.X vmlinuz vmlinux.bin

DISTFILES	= second/head.S second/MMU030.c second/MMU040.c second/main.c \
		  second/MMU030.h second/MMU040.h second/console.c \
		  second/Makefile second/console.h second/printf.c \
		  second/MMU030_asm.S second/MMU040_asm.S second/uncompress.h \
		  second/font_8x16.c second/ld.script second/memory.c \
		  second/inflate.c second/uncompress.c second/misc.c \
		  second/bootinfo.h second/misc.h second/lowmem.h \
		  second/bootinfo.c second/glue.h second/memory.h \
		  second/glue.S second/enter_kernel030.S \
		  second/load.h second/load.c \
		  second/enter_kernel040.S first/first.S \
		  first/Makefile second/bank.c second/bank.h second/arch.h \
		  second/arch.c Makefile COPYING README AUTHORS ChangeLog \
		  tools/Makefile tools/emile-first.h tools/emile-second.h \
		  tools/emile-set-cmdline.c tools/emile-first-info.c \
		  tools/emile-first-tune.c tools/emile.h \
		  tools/emile-install.c second/copymem.i

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
