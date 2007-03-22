#
#  (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
#
#

MAKEFLAGS += --no-print-directory

PACKAGE	= emile
VERSION	= 0.11CVS

PREFIX=/

# kernel boot arguments

#FLOPPY=/dev/floppy/0
FLOPPY=/dev/sda
#CONSOLE=console=ttyS0,9600n8 console=tty0

NETBOOT_ARGS="root=/dev/nfs ip=dhcp rw $(CONSOLE)"
RESCUE_ARGS="root=/dev/ramdisk ramdisk_size=2048 $(CONSOLE)"
CDBOOT_ARGS="root=/dev/ramdisk ramdisk_size=13000 $(CONSOLE)"
INSTALLER_ARGS="prompt_ramdisk=1 load_ramdisk=1 ramdisk_start=0 root=/dev/fd0 ramdisk_size=4096 $(CONSOLE)"
BOOT_ARGS="root=/dev/sda4 $(CONSOLE)"

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
STRIP=$(CROSS_COMPILE)strip

ifneq ($(ARCH),m68k)
M68K_CROSS_COMPILE	= m68k-linux-
M68K_GCC_VERSION=$(shell $(M68K_CROSS_COMPILE)gcc -dumpversion 2> /dev/null)
ifeq ($(M68K_GCC_VERSION), )
  M68K_CROSS_COMPILE	= m68k-linux-gnu-
  M68K_GCC_VERSION=$(shell $(M68K_CROSS_COMPILE)gcc -dumpversion 2> /dev/null)
endif
ifeq ($(M68K_GCC_VERSION), )
$(error Cannot find m68k cross-compiler $(M68K_GCC_VERSION))
endif
$(info m68k cross-compiler is $(M68K_CROSS_COMPILE)gcc $(M68K_GCC_VERSION))
endif

M68K_AS=$(M68K_CROSS_COMPILE)as
M68K_CC=$(M68K_CROSS_COMPILE)gcc
M68K_LD=$(M68K_CROSS_COMPILE)ld
M68K_OBJCOPY=$(M68K_CROSS_COMPILE)objcopy
M68K_STRIP=$(M68K_CROSS_COMPILE)strip

ifneq ($(ARCH),ppc)
PPC_CROSS_COMPILE	= powerpc-linux-
PPC_GCC_VERSIONS		= $(shell $(PPC_CROSS_COMPILE)gcc -dumpversion 2> /dev/null)
ifeq ($(PPC_GCC_VERSION), )
  PPC_CROSS_COMPILE	= powerpc-linux-gnu-
  PPC_GCC_VERSION=$(shell $(PPC_CROSS_COMPILE)gcc -dumpversion 2> /dev/null)
endif
ifeq ($(PPC_GCC_VERSION), )
    $(error Cannot find ppc cross-compiler $(PPC_GCC_VERSION))
endif
$(info ppc cross-compiler is $(PPC_CROSS_COMPILE)gcc $(PPC_GCC_VERSION))
endif

PPC_AS=$(PPC_CROSS_COMPILE)as
PPC_CC=$(PPC_CROSS_COMPILE)gcc
PPC_LD=$(PPC_CROSS_COMPILE)ld
PPC_OBJCOPY=$(PPC_CROSS_COMPILE)objcopy
PPC_STRIP=$(PPC_CROSS_COMPILE)strip

# Kernel architecture

LINUXRAMDISK=ramdisk.gz
LINUXPATH=vmlinux

LINUX=$(shell ls $(LINUXPATH) 2> /dev/null)

ifeq ($(LINUX),$(LINUXPATH))
	FILEARCH=$(shell file -bknL $(LINUX) | cut -d, -f 2)
	ifeq ($(findstring PowerPC, $(FILEARCH)), PowerPC)

		KARCH=ppc
		KSTRIP=$(PPC_STRIP)

	else
	ifeq ($(findstring Motorola 68000, $(FILEARCH)), Motorola 68000)

		KARCH=classic
		KSTRIP=$(M68K_STRIP)

	else
	ifeq ($(findstring Motorola 68, $(FILEARCH)), Motorola 68)

		KARCH=m68k
		KSTRIP=$(M68K_STRIP)
	else
		KARCH=unknown
		KSTRIP=$(M68K_STRIP)
	endif
	endif
	endif
else
	KARCH=m68k
	KSTRIP=$(M68K_STRIP)
endif

# Target

.PHONY: first libemile libblock libiso9660 libiso9660-m68k libgzip-m68k tools \
       libcontainer clean all_bin all install tools-install first-install docs-install \
       uninstall tools-uninstall first-uninstall docs-uninstall \
       clean libemile-clean libmacos-clean libunix-clean tools-clean \
       first-clean second-clean docs-clean libiso9660-clean libgzip-clean \
       libfloppy-clean libscsi-clean libstream-clean libblock-clean dist docs \
       apple_driver apple_driver_clean

all: docs libemile libblock libiso9660 libiso9660-m68k libgzip-m68k \
     tools first libstream libcontainer \
     second/$(KARCH)-linux-floppy/second \
     second/$(KARCH)-linux-scsi/second second/m68k-netbsd-floppy/second \
     apple_driver

# We can build floppy image only if a kernel is provided

ifeq ($(LINUX),$(LINUXPATH))
all_bin: netboot.bin rescue.bin debian-installer.bin boot.bin \
	 cdboot-sarge.bin cdboot-woody.bin second/$(KARCH)-linux-scsi/second \
	 apple_driver
	rm -f last.bin

cdboot-woody.bin: tools first second/$(KARCH)-linux-scsi/second
	tools/emile-install -f first/first_floppy \
			    -s second/$(KARCH)-linux-scsi/second \
			    -k "iso9660:(sd3)/install/mac/linux.bin" \
			    -r "iso9660:(sd3)/install/mac/root.bin" \
			    cdboot-woody.bin.X
	tools/emile-set-cmdline -a $(CDBOOT_ARGS) cdboot-woody.bin.X
	mv cdboot-woody.bin.X cdboot-woody.bin
	rm -f last.bin
	ln -s cdboot-woody.bin last.bin

cdboot-sarge.bin: tools first second/$(KARCH)-linux-scsi/second
	tools/emile-install -f first/first_floppy \
			-s second/$(KARCH)-linux-scsi/second \
			-k "iso9660:(sd3)/install/kernels/vmlinuz-2.2.25-mac" \
			-r "iso9660:(sd3)/install/cdrom/initrd22.gz" \
			    cdboot-sarge.bin.X
	tools/emile-set-cmdline -a $(CDBOOT_ARGS) cdboot-sarge.bin.X
	mv cdboot-sarge.bin.X cdboot-sarge.bin
	rm -f last.bin
	ln -s cdboot-sarge.bin last.bin

floppy.bin: tools first vmlinuz \
	    second/$(KARCH)-linux-floppy/second
	tools/emile-install -f first/first_floppy \
			    -s second/$(KARCH)-linux-floppy/second \
			    -k vmlinuz floppy.bin.X
ifdef CONSOLE
	tools/emile-set-output floppy.bin.X --printer --modem
endif
	mv floppy.bin.X floppy.bin

floppy_ramdisk.bin: tools first vmlinuz \
		    second/$(KARCH)-linux-floppy/second $(LINUXRAMDISK)
	tools/emile-install -f first/first_floppy  \
			    -s second/$(KARCH)-linux-floppy/second \
			    -k vmlinuz -r $(LINUXRAMDISK) floppy_ramdisk.bin.X
ifdef CONSOLE
	tools/emile-set-output floppy_ramdisk.bin.X --printer --modem
endif
	mv floppy_ramdisk.bin.X floppy_ramdisk.bin

debian-installer.bin: floppy.bin
	rm -f last.bin
	cp floppy.bin debian-installer.bin.X
	tools/emile-set-cmdline -a $(INSTALLER_ARGS) debian-installer.bin.X
	mv debian-installer.bin.X debian-installer.bin
	ln -s debian-installer.bin last.bin

netboot.bin: floppy.bin
	rm -f last.bin
	cp floppy.bin netboot.bin.X
	tools/emile-set-cmdline -a $(NETBOOT_ARGS) netboot.bin.X
	mv netboot.bin.X netboot.bin
	ln -s netboot.bin last.bin

rescue.bin: floppy_ramdisk.bin
	rm -f last.bin
	cp floppy_ramdisk.bin rescue.bin.X
	tools/emile-set-cmdline -a $(RESCUE_ARGS) rescue.bin.X
	mv rescue.bin.X rescue.bin
	ln -s rescue.bin last.bin

boot.bin: floppy.bin
	rm -f last.bin
	cp floppy.bin boot.bin.X
	tools/emile-set-cmdline -a $(BOOT_ARGS) boot.bin.X
	mv boot.bin.X boot.bin
	ln -s boot.bin last.bin

vmlinuz: $(LINUX)
	$(KSTRIP) -s $(LINUX) -o $(LINUX).stripped
	gzip -9 $(LINUX).stripped
	mv $(LINUX).stripped.gz vmlinuz
endif

NETBSDPATH=netbsd

NETBSD=$(shell ls $(NETBSDPATH) 2> /dev/null)

ifeq ($(NETBSD),$(NETBSDPATH))
netbsd-floppy.bin: tools first netbsd.gz \
	    second/m68k-netbsd-floppy/second
	tools/emile-install -f first/first_floppy \
			    -s second/$(KARCH)-netbsd-floppy/second \
			    -k netbsd.gz netbsd-floppy.bin.X
ifdef CONSOLE
	tools/emile-set-output netbsd-floppy.bin.X --printer --modem
endif
	mv netbsd-floppy.bin.X netbsd-floppy.bin

netbsd-boot.bin: netbsd-floppy.bin
	rm -f last.bin
	cp netbsd-floppy.bin netbsd-boot.bin.X
	tools/emile-set-cmdline -a $(BOOT_ARGS) netbsd-boot.bin.X
	mv netbsd-boot.bin.X netbsd-boot.bin
	ln -s netbsd-boot.bin last.bin

netbsd.gz: $(NETBSD)
	$(KSTRIP) -s $(NETBSD) -o $(NETBSD).stripped
	gzip -9 $(NETBSD).stripped
	mv $(NETBSD).stripped.gz netbsd.gz
endif

export SIGNATURE VERSION DESTDIR PREFIX KARCH CROSS_COMPILE

first::
	$(MAKE) -C first OBJCOPY=$(M68K_OBJCOPY) AS=$(M68K_AS)

apple_driver::
	$(MAKE) -C apple_driver OBJCOPY=$(M68K_OBJCOPY) AS=$(M68K_AS) \
		LD=$(M68K_LD) CC=$(M68K_CC)

second/$(KARCH)-linux-floppy/second:: libmacos libunix libiso9660-m68k libgzip-m68k libfloppy libscsi libstream libblock libcontainer
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) PPC_OBJCOPY=$(PPC_OBJCOPY) PPC_CC=$(PPC_CC) \
		MEDIA=floppy TARGET=$(KARCH)-linux

second/$(KARCH)-linux-scsi/second:: libmacos libunix libiso9660-m68k libgzip-m68k libfloppy libscsi libstream libblock libcontainer
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) PPC_OBJCOPY=$(PPC_OBJCOPY) PPC_CC=$(PPC_CC) \
		TARGET=$(KARCH)-linux MEDIA=scsi

second/m68k-netbsd-floppy/second:: libmacos libunix libiso9660-m68k libgzip-m68k libfloppy libstream libblock libcontainer
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) TARGET=m68k-netbsd MEDIA=floppy

first-install::
	$(MAKE) -C first install

first-uninstall::
	$(MAKE) -C first uninstall

second-install::
	$(MAKE) -C second install 

second-uninstall::
	$(MAKE) -C second uninstall

libmacos::
	$(MAKE) -C libmacos all CC=$(M68K_CC) AS=$(M68K_AS)

libunix::
	$(MAKE) -C libunix all CC=$(M68K_CC) AS=$(M68K_AS)

libiso9660-m68k::
	$(MAKE) -C libiso9660 all LD=$(M68K_LD) CC=$(M68K_CC) AS=$(M68K_AS) \
		TARGET=m68k-linux

libiso9660::
	$(MAKE) -C libiso9660 all TARGET=native CROSS_COMPILE=$(CROSS_COMPILE)

libcontainer::
	$(MAKE) -C libcontainer all LD=$(M68K_LD) CC=$(M68K_CC) AS=$(M68K_AS)

libblock::
	$(MAKE) -C libblock all LD=$(M68K_LD) CC=$(M68K_CC) AS=$(M68K_AS)

libgzip-m68k::
	$(MAKE) -C libgzip all TARGET=$(KARCH)-linux LD=$(M68K_LD) CC=$(M68K_CC) AS=$(M68K_AS) TARGET=m68k-linux

libgzip::
	$(MAKE) -C libgzip all TARGET=native CROSS_COMPILE=$(CROSS_COMPILE)

libemile::
	$(MAKE) -C libemile all CROSS_COMPILE=$(CROSS_COMPILE)

libfloppy::
	$(MAKE) -C libfloppy all CC=$(M68K_CC) AS=$(M68K_AS)

libscsi::
	$(MAKE) -C libscsi all CC=$(M68K_CC) AS=$(M68K_AS)

libstream::
	$(MAKE) -C libstream all CC=$(M68K_CC) AS=$(M68K_AS)

tools::  libemile libiso9660 libgzip
	$(MAKE) -C tools all CROSS_COMPILE=$(CROSS_COMPILE)

tools-install:: tools
	$(MAKE) -C tools install

tools-uninstall::
	$(MAKE) -C tools uninstall

docs-install:: docs
	$(MAKE) -C docs install

docs-uninstall::
	$(MAKE) -C docs uninstall

apple_driver-install::
	$(MAKE) -C apple_driver install

apple_driver-uninstall::
	$(MAKE) -C apple_driver uninstall

docs::
	$(MAKE) -C docs all

dump: last.bin
	dd if=last.bin of=$(FLOPPY) bs=512

install: tools-install first-install second-install docs-install apple_driver-install

uninstall: tools-uninstall first-uninstall second-uninstall docs-uninstall

libemile-clean:
	$(MAKE) -C libemile clean

libmacos-clean:
	$(MAKE) -C libmacos clean

libunix-clean:
	$(MAKE) -C libunix clean

libscsi-clean:
	$(MAKE) -C libscsi clean

libstream-clean:
	$(MAKE) -C libstream clean

libiso9660-clean::
	$(MAKE) -C libiso9660 clean TARGET=native
	$(MAKE) -C libiso9660 clean TARGET=$(KARCH)-linux

libcontainer-clean::
	$(MAKE) -C libcontainer clean

libblock-clean::
	$(MAKE) -C libblock clean

libgzip-clean::
	$(MAKE) -C libgzip clean TARGET=native
	$(MAKE) -C libgzip clean TARGET=$(KARCH)-linux

tools-clean:
	$(MAKE) -C tools clean

first-clean:
	$(MAKE) -C first clean

second-clean:
	$(MAKE) -C second clean

docs-clean:
	$(MAKE) -C docs clean

libfloppy-clean:
	$(MAKE) -C libfloppy clean

apple_driver-clean:
	$(MAKE) -C apple_driver clean

clean:: libemile-clean libmacos-clean libunix-clean tools-clean first-clean \
	second-clean docs-clean libiso9660-clean libgzip-clean libfloppy-clean \
	libscsi-clean libstream-clean libblock-clean libcontainer-clean \
	apple_driver-clean
	rm -f floppy.bin floppy.bin.X floppy_ramdisk.bin \
	      floppy_ramdisk.bin.X rescue.bin rescue.bin.X \
	      debian-installer.bin debian-installer.bin.X \
	      netboot.bin netboot.bin.X boot.bin boot.bin.X \
	      vmlinuz last.bin cdboot-sarge.bin cdboot-woody.bin

DISTFILES = AUTHORS ChangeLog COPYING Makefile README README.floppy \
	    README.scsi Rules.mk

dist:
	rm -fr $(PACKAGE)-$(VERSION)
	mkdir $(PACKAGE)-$(VERSION)
	@$(MAKE) -C tools dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libemile dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C second dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C first dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C docs dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libmacos dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libunix dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libstream dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libscsi dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libfloppy dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libblock dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libiso9660 dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libcontainer dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libgzip dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C tools dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C debian dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C apple_driver dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@echo TAR emile
	@for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(PACKAGE)-$(VERSION)/$$dir; \
		fi; \
		cp -p $$file $(PACKAGE)-$(VERSION)/$$file; \
	done
	@rm -f $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION).tar.bz2
	@tar cf $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION)
	@bzip2 -9 $(PACKAGE)-$(VERSION).tar
	@rm -fr $(PACKAGE)-$(VERSION)
