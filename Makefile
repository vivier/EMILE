#
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PACKAGE	= emile
VERSION	= 0.7CVS

PREFIX=/

# kernel boot arguments

RAMDISK=$(shell ls ramdisk.gz 2> /dev/null)

ifeq ($(RAMDISK),ramdisk.gz)
#KERNEL_ARGS="root=/dev/ramdisk ramdisk_size=2048"
KERNEL_ARGS="root=/dev/ramdisk ramdisk_size=2048 console=ttyS0,9600n8 console=tty0"
else
# NFS boot
#KERNEL_ARGS="root=/dev/nfs ip=dhcp nfsroot=192.168.100.1:/nfsroot rw"
# SCSI boot
KERNEL_ARGS="root=/dev/sda3"
#KERNEL_ARGS="prompt_ramdisk=1 load_ramdisk=1 ramdisk_start=0 root=/dev/fd0 ramdisk_size=4096"
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

all: libemile tools first/first_floppy second/second_floppy

floppy.img: libemile tools first/first_floppy vmlinuz second/second_floppy \
	    $(RAMDISK)
ifeq ($(RAMDISK),ramdisk.gz)
	tools/emile-install -f first/first_floppy  -s second/second_floppy \
			    -i vmlinuz -b $(KERNEL_SIZE) \
			    -r $(RAMDISK) \
			     floppy.img.X
else
	tools/emile-install -f first/first_floppy -s second/second_floppy \
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

first/first_floppy::
	$(MAKE) -C first OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) SIGNATURE="$(SIGNATURE)"

second/second_floppy::
	$(MAKE) -C second OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) \
		VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)"

libemile::
	$(MAKE) -C libemile all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)"

tools::
	$(MAKE) -C tools all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)"


dump: floppy.img
	dd if=floppy.img of=/dev/fd0 bs=512
	eject /dev/fd0

install:
	$(MAKE) -C libemile DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) install
	$(MAKE) -C tools DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) install
	$(MAKE) -C first DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) install
	$(MAKE) -C second DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) install

clean:
	$(MAKE) -C libemile clean
	$(MAKE) -C tools clean
	$(MAKE) -C first clean
	$(MAKE) -C second clean
	rm -f floppy.img floppy.img.X vmlinuz vmlinux.bin

FIRST_FILES	= first/Makefile first/first.S

SECOND_FILES	= second/MMU030.c second/MMU030.h second/MMU030_asm.S \
		  second/MMU040.c second/MMU040.h second/MMU040_asm.S \
		  second/Makefile second/arch.c second/scsi.h second/arch.h \
		  second/bank.c second/bank.h second/bootinfo.c \
		  second/bootinfo.h second/console.c second/console.h \
		  second/copymem.i second/container.S second/font_8x16.c \
		  second/glue.S second/glue.h second/head.S second/inflate.c \
		  second/ld.script second/lowmem.h second/memory.c \
		  second/memory.h second/main.c second/load.c second/load.h \
		  second/scsi.c second/enter_kernel030.S second/serial.c \
		  second/serial.h second/vga.c second/vga.h second/head.h \
		  second/misc.c second/misc.h second/printf.c \
		  second/uncompress.c second/uncompress.h \
		  second/enter_kernel040.S

TOOLS_FILES	= tools/emile-set-cmdline.c tools/Makefile \
		  tools/emile-first-info.c tools/emile-first-tune.c \
		  tools/emile-second.h tools/emile-install.c \
		  tools/emile-set-output.c tools/emile.c

LIB_FILES	= bootblock.h emile_first_get_param.c emile_first_set_param.c \
		  emile_first_set_param_scsi.c emile_floppy_create_image.c \
		  emile.h emile_map_bootblock_is_valid.c emile_map_close.c \
		  emile_map_geometry.c emile_map_get_driver_info.c \
		  emile_map_get_driver_number.c \
		  emile_map_get_partition_geometry.c \
		  emile_map_get_partition_name.c \
		  emile_map_get_partition_type.c emile_map_is_valid.c \
		  emile_map_open.c emile_map_partition_is_bootable.c \
		  emile_map_partition_is_startup.c \
		  emile_map_partition_is_valid.c \
		  emile_map_partition_set_bootable.c \
		  emile_map_partition_set_startup.c \
		  emile_map_read_bootblock.c emile_map_read.c \
		  emile_map_set_partition_name.c \
		  emile_map_set_partition_type.c emile_map_write_bootblock.c \
		  emile_map_write.c emile_scsi_create_container.c \
		  emile_second_get_buffer_size.c emile_second_get_cmdline.c \
		  emile_second_get_kernel.c emile_second_get_output.c \
		  emile_second_set_buffer_size.c emile_second_set_cmdline.c \
		  emile_second_set_kernel.c emile_second_set_kernel_scsi.c \
		  emile_second_set_output.c libemile.h Makefile partition.h

DISTFILES	= $(FIRST_FILES) $(SECOND_FILES) $(LIB_FILES) $(TOOLS_FILES)

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
