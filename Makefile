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

all: libemile tools first/first_floppy second/second_floppy

floppy.img: libemile tools first/first_floppy vmlinuz second/second_floppy \
	    $(RAMDISK)
ifeq ($(RAMDISK),ramdisk.gz)
	tools/emile-install -f first/first_floppy  -s second/second_floppy \
			    -k vmlinuz -r $(RAMDISK) floppy.img.X
else
	tools/emile-install -f first/first_floppy -s second/second_floppy \
			    -k vmlinuz floppy.img.X
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
	$(MAKE) -C tools all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
			     PREFIX=$(PREFIX)


dump: floppy.img
	dd if=floppy.img of=/dev/fd0 bs=512
	eject /dev/fd0

install: all
	install -d $(DESTDIR)/$(PREFIX)/usr/include/
	install libemile/libemile.h $(DESTDIR)/$(PREFIX)/usr/include/libemile.h
	install -d $(DESTDIR)/$(PREFIX)/usr/lib/
	install libemile/libemile.a $(DESTDIR)/$(PREFIX)/usr/lib/libemile.a
	install -d $(DESTDIR)/$(PREFIX)/sbin/
	install tools/emile-set-cmdline $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	install tools/emile-first-tune $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	install tools/emile-install $(DESTDIR)/$(PREFIX)/sbin/emile-install
	install tools/emile-set-output $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	install tools/emile $(DESTDIR)/$(PREFIX)/sbin/emile
	install tools/emile-map-set $(DESTDIR)/$(PREFIX)/sbin/emile-map-set
	install -d $(DESTDIR)/$(PREFIX)/boot/emile/
	install first/first_scsi $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install first/first_floppy $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy
	install -d $(DESTDIR)/$(PREFIX)/boot/emile/
	install second/second_scsi $(DESTDIR)/$(PREFIX)/boot/emile/second_scsi
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install second/second_floppy $(DESTDIR)/$(PREFIX)/lib/emile/second_floppy

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/usr/include/libemile.h
	rm -f $(DESTDIR)/$(PREFIX)/usr/lib/libemile.a
	rm -fr $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	rm -fr $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	rm -fr $(DESTDIR)/$(PREFIX)/sbin/emile-install
	rm -fr $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	rm -fr $(DESTDIR)/$(PREFIX)/sbin/emile
	rm -fr $(DESTDIR)/$(PREFIX)/sbin/emile-map-set
	rm -f $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy
	rm -f $(DESTDIR)/$(PREFIX)/boot/emile/second_scsi
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/second_floppy

clean:
	$(MAKE) -C libemile clean
	$(MAKE) -C tools clean
	$(MAKE) -C first clean
	$(MAKE) -C second clean
	rm -f floppy.img floppy.img.X vmlinuz vmlinux.bin

MAIN_FILES	= AUTHORS ChangeLog COPYING Makefile README

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
		  tools/emile-first-tune.c \
		  tools/emile-second.h tools/emile-install.c \
		  tools/emile-set-output.c tools/emile.c tools/emile_scanbus.c \
		  tools/emile-map-set.c

LIB_FILES	= libemile/bootblock.h libemile/emile_first_get_param.c \
		  libemile/Makefile libemile/emile_first_set_param.c \
		  libemile/emile_first_set_param_scsi.c \
		  libemile/emile_floppy_create_image.c \
		  libemile/emile.h libemile/emile_map_bootblock_get_type.c \
		  libemile/emile_map_bootblock_is_valid.c \
		  libemile/emile_map_bootblock_read.c \
		  libemile/emile_map_bootblock_write.c \
		  libemile/emile_map_close.c \
		  libemile/emile_map_geometry.c \
		  libemile/emile_map_get_driver_info.c \
		  libemile/emile_map_get_driver_number.c \
		  libemile/emile_map_get_number.c \
		  libemile/emile_map_get_partition_geometry.c \
		  libemile/emile_map_get_partition_name.c \
		  libemile/emile_map_get_partition_type.c \
		  libemile/emile_map_is_valid.c \
		  libemile/emile_map_open.c \
		  libemile/emile_map_partition_get_flags.c \
		  libemile/emile_map_partition_is_bootable.c \
		  libemile/emile_map_partition_is_startup.c \
		  libemile/emile_map_partition_is_valid.c \
		  libemile/emile_map_partition_set_bootable.c \
		  libemile/emile_map_partition_set_startup.c \
		  libemile/emile_map_read.c \
		  libemile/emile_map_set_partition_name.c \
		  libemile/emile_map_set_partition_type.c \
		  libemile/emile_map_set_startup.c \
		  libemile/emile_map_write.c \
		  libemile/emile_scsi_create_container.c \
		  libemile/emile_scsi_get_dev.c \
		  libemile/emile_scsi_get_rdev.c \
		  libemile/emile_second_get_buffer_size.c \
		  libemile/emile_second_get_cmdline.c \
		  libemile/emile_second_get_kernel.c \
		  libemile/emile_second_get_output.c \
		  libemile/emile_second_set_buffer_size.c \
		  libemile/emile_second_set_cmdline.c \
		  libemile/emile_second_set_kernel.c \
		  libemile/emile_second_set_kernel_scsi.c \
		  libemile/emile_second_set_output.c libemile/libemile.h \
		  libemile/partition.h libemile/emile_map_has_apple_driver.c \
		  libemile/emile_map_seek_driver_partition.c \
		  libemile/emile_get_uncompressed_size.c

DISTFILES	= $(MAIN_FILES) $(FIRST_FILES) $(SECOND_FILES) $(LIB_FILES) \
		  $(TOOLS_FILES)

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
