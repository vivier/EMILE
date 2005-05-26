#
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PACKAGE	= emile
VERSION	= 0.9CVS

PREFIX=/

# kernel boot arguments

FLOPPY=/dev/floppy/0
#CONSOLE=console=ttyS0,9600n8 console=tty0

NETBOOT_ARGS="root=/dev/nfs ip=dhcp rw $(CONSOLE)"
RESCUE_ARGS="root=/dev/ramdisk ramdisk_size=2048 $(CONSOLE)"
INSTALLER_ARGS="prompt_ramdisk=1 load_ramdisk=1 ramdisk_start=0 root=/dev/fd0 ramdisk_size=4096 $(CONSOLE)"
BOOT_ARGS="root=/dev/sda3 $(CONSOLE)"

# build info

WHO	= $(shell whoami)
WHERE	= $(shell hostname)
WHEN	= $(shell LANG=C date)
ARCH	= $(shell uname -m)
OS	= $(shell uname -o)

SIGNATURE = $(PACKAGE)-$(VERSION) $(WHO)@$(WHERE)($(ARCH) $(OS)) $(WHEN)

# tools to use

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy

ifneq ($(ARCH),m68k)
M68K_CROSS_COMPILE	= m68k-linux-
endif

M68K_AS=$(M68K_CROSS_COMPILE)as
M68K_CC=$(M68K_CROSS_COMPILE)gcc
M68K_LD=$(M68K_CROSS_COMPILE)ld
M68K_OBJCOPY=$(M68K_CROSS_COMPILE)objcopy

ifneq ($(ARCH),ppc)
PPC_CROSS_COMPILE	= ppc-linux-
endif

PPC_AS=$(PPC_CROSS_COMPILE)as
PPC_CC=$(PPC_CROSS_COMPILE)gcc
PPC_LD=$(PPC_CROSS_COMPILE)ld
PPC_OBJCOPY=$(PPC_CROSS_COMPILE)objcopy

# Kernel architecture

RAMDISK=ramdisk.gz
KERNELPATH=vmlinux

KERNEL=$(shell ls $(KERNELPATH) 2> /dev/null)

ifeq ($(KERNEL),$(KERNELPATH))
FILEARCH=$(shell file -bknL $(KERNEL) | cut -d, -f 2)
ifeq ($(findstring PowerPC, $(FILEARCH)), PowerPC)
KARCH=ppc
else
ifeq ($(findstring Motorola 68, $(FILEARCH)), Motorola 68)
KARCH=m68k
else
KARCH=unknown
endif
endif
else
KARCH=m68k
endif

# Target

all: libemile tools first/first_floppy second/$(KARCH)-second_floppy \
     second/$(KARCH)-second_scsi

# We can build floppy image only if a kernel is provided

ifeq ($(KERNEL),$(KERNELPATH))
all_bin: netboot.bin rescue.bin debian-installer.bin boot.bin
	rm -f last.bin

floppy.bin: libemile tools first/first_floppy vmlinuz \
	    second/$(KARCH)-second_floppy
	tools/emile-install -f first/first_floppy \
			    -s second/$(KARCH)-second_floppy \
			    -k vmlinuz floppy.bin.X
ifdef CONSOLE
	tools/emile-set-output floppy.bin.X --printer --modem
endif
	mv floppy.bin.X floppy.bin

floppy_ramdisk.bin: libemile tools first/first_floppy vmlinuz \
		    second/$(KARCH)-second_floppy  $(RAMDISK)
	tools/emile-install -f first/first_floppy  \
			    -s second/$(KARCH)-second_floppy \
			    -k vmlinuz -r $(RAMDISK) floppy_ramdisk.bin.X
ifdef CONSOLE
	tools/emile-set-output floppy_ramdisk.bin.X --printer --modem
endif
	mv floppy_ramdisk.bin.X floppy_ramdisk.bin

debian-installer.bin: floppy.bin
	rm -f last.bin
	cp floppy.bin debian-installer.bin.X
	tools/emile-set-cmdline debian-installer.bin.X $(INSTALLER_ARGS)
	mv debian-installer.bin.X debian-installer.bin
	ln -s debian-installer.bin last.bin

netboot.bin: floppy.bin
	rm -f last.bin
	cp floppy.bin netboot.bin.X
	tools/emile-set-cmdline netboot.bin.X $(NETBOOT_ARGS)
	mv netboot.bin.X netboot.bin
	ln -s netboot.bin last.bin

rescue.bin: floppy_ramdisk.bin
	rm -f last.bin
	cp floppy_ramdisk.bin rescue.bin.X
	tools/emile-set-cmdline rescue.bin.X $(RESCUE_ARGS)
	mv rescue.bin.X rescue.bin
	ln -s rescue.bin last.bin

boot.bin: floppy.bin
	rm -f last.bin
	cp floppy.bin boot.bin.X
	tools/emile-set-cmdline boot.bin.X $(BOOT_ARGS)
	mv boot.bin.X boot.bin
	ln -s boot.bin last.bin

vmlinux.bin: $(KERNEL)
	$(M68K_OBJCOPY) -I elf32-big -O binary -R .note -R .comment -S $(KERNEL) vmlinux.bin

vmlinuz: vmlinux.bin
	cp vmlinux.bin vmlinuz.out
	gzip -9 vmlinuz.out
	mv vmlinuz.out.gz vmlinuz
endif

first/first_floppy::
	$(MAKE) -C first OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) AS=$(M68K_AS) SIGNATURE="$(SIGNATURE)"

second/$(KARCH)-second_floppy::
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
		$(KARCH)-second_floppy

second/$(KARCH)-second_scsi::
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
		$(KARCH)-second_scsi

libemile::
	$(MAKE) -C libemile all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
		CROSS_COMPILE=$(CROSS_COMPILE)

tools::
	$(MAKE) -C tools all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
			     PREFIX=$(PREFIX) CROSS_COMPILE=$(CROSS_COMPILE)


dump: last.bin
	dd if=last.bin of=$(FLOPPY) bs=512
	# eject makes hanging my USB floppy device
	#eject $(FLOPPY)

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
	install second/$(KARCH)-second_scsi $(DESTDIR)/$(PREFIX)/boot/emile/$(KARCH)-second_scsi
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install second/$(KARCH)-second_floppy $(DESTDIR)/$(PREFIX)/lib/emile/$(KARCH)-second_floppy

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
	rm -f $(DESTDIR)/$(PREFIX)/boot/emile/$(KARCH)-second_scsi
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/$(KARCH)-second_floppy

clean:
	$(MAKE) -C libemile clean
	$(MAKE) -C tools clean
	$(MAKE) -C first clean
	$(MAKE) -C second clean
	rm -f floppy.bin floppy.bin.X floppy_ramdisk.bin \
	      floppy_ramdisk.bin.X rescue.bin rescue.bin.X \
	      debian-installer.bin debian-installer.bin.X \
	      netboot.bin netboot.bin.X boot.bin boot.bin.X \
	      vmlinuz vmlinux.bin last.bin

MAIN_FILES	= AUTHORS ChangeLog COPYING Makefile README README.floppy \
		  README.scsi

FIRST_FILES	= first/Makefile first/first.S

SECOND_FILES	= second/MMU030.c second/MMU030.h second/MMU030_asm.S \
		  second/MMU040.c second/MMU040.h second/MMU040_asm.S \
		  second/PPC_asm.S second/bootx.h second/bootx.c \
		  second/Makefile second/arch.c second/scsi.h second/arch.h \
		  second/bank.c second/bank.h second/bootinfo.c \
		  second/bootinfo.h second/console.c second/console.h \
		  second/copymem.i second/container.S second/font_8x16.c \
		  second/glue.S second/glue.h second/head.S second/inflate.c \
		  second/ld.script second/lowmem.h second/memory.c \
		  second/memory.h second/main.c second/load.c second/load.h \
		  second/scsi.c second/enter_kernel030.S second/serial.c \
		  second/enter_kernelPPC.S second/enter_kernelPPC.h \
		  second/enter_kernelnoMMU.S \
		  second/serial.h second/vga.c second/vga.h second/head.h \
		  second/misc.c second/misc.h second/printf.c \
		  second/uncompress.c second/uncompress.h \
		  second/enter_kernel040.S

TOOLS_FILES	= tools/emile-set-cmdline.c tools/Makefile \
		  tools/emile-first-tune.c \
		  tools/emile-install.c \
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
		  libemile/emile_map_partition_set_flags.c \
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
		  libemile/emile_get_uncompressed_size.c \
		  libemile/emile_map_get_bootinfo.c libemile/emile_map_dev.c \
		  libemile/emile_checksum.c \
		  libemile/emile_map_set_driver_number.c \
		  libemile/emile_map_set_driver_info.c \
		  libemile/emile_map_set_bootinfo.c \
		  libemile/emile_block0_write.c

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
