#
#  (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PACKAGE	= emile
VERSION	= 0.10CVS

PREFIX=/

# kernel boot arguments

FLOPPY=/dev/floppy/0
#CONSOLE=console=ttyS0,9600n8 console=tty0

NETBOOT_ARGS="root=/dev/nfs ip=dhcp rw $(CONSOLE)"
#RESCUE_ARGS="root=/dev/ramdisk ramdisk_size=2048 $(CONSOLE)"
RESCUE_ARGS="root=/dev/ramdisk ramdisk_size=13000 $(CONSOLE)"
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
endif

M68K_AS=$(M68K_CROSS_COMPILE)as
M68K_CC=$(M68K_CROSS_COMPILE)gcc
M68K_LD=$(M68K_CROSS_COMPILE)ld
M68K_OBJCOPY=$(M68K_CROSS_COMPILE)objcopy
M68K_STRIP=$(M68K_CROSS_COMPILE)strip

ifneq ($(ARCH),ppc)
PPC_CROSS_COMPILE	= ppc-linux-
endif

PPC_AS=$(PPC_CROSS_COMPILE)as
PPC_CC=$(PPC_CROSS_COMPILE)gcc
PPC_LD=$(PPC_CROSS_COMPILE)ld
PPC_OBJCOPY=$(PPC_CROSS_COMPILE)objcopy

# Kernel architecture

LINUXRAMDISK=ramdisk.gz
LINUXPATH=vmlinux

LINUX=$(shell ls $(LINUXPATH) 2> /dev/null)

ifeq ($(LINUX),$(LINUXPATH))
	FILEARCH=$(shell file -bknL $(LINUX) | cut -d, -f 2)
	ifeq ($(findstring PowerPC, $(FILEARCH)), PowerPC)

		KARCH=ppc

	else
	ifeq ($(findstring Motorola 68000, $(FILEARCH)), Motorola 68000)

		KARCH=classic

	else
	ifeq ($(findstring Motorola 68, $(FILEARCH)), Motorola 68)

		KARCH=m68k
	else
		KARCH=unknown
	endif
	endif
	endif
else
	KARCH=m68k
endif

# Target

.PHONY: first libemile libblock libiso9660 libiso9660-m68k libgzip-m68k tools \
       clean all_bin all install tools-install first-install docs-install \
       uninstall tools-uninstall first-uninstall docs-uninstall \
       clean libemile-clean libmacos-clean libunix-clean tools-clean \
       first-clean second-clean docs-clean libiso9660-clean libgzip-clean \
       libfloppy-clean libscsi-clean libstream-clean libblock-clean dist docs

all: docs libemile libblock libiso9660 libiso9660-m68k libgzip-m68k \
     tools first libstream \
     second/$(KARCH)-linux-floppy/second \
     second/$(KARCH)-linux-scsi/second second/m68k-netbsd-floppy/second

# We can build floppy image only if a kernel is provided

ifeq ($(LINUX),$(LINUXPATH))
all_bin: netboot.bin rescue.bin debian-installer.bin boot.bin \
	 second/$(KARCH)-linux-scsi/second
	rm -f last.bin

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

vmlinuz: $(LINUX)
	$(M68K_STRIP) -s $(LINUX) -o $(LINUX).stripped
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
	tools/emile-set-cmdline netbsd-boot.bin.X $(BOOT_ARGS)
	mv netbsd-boot.bin.X netbsd-boot.bin
	ln -s netbsd-boot.bin last.bin

netbsd.gz: $(NETBSD)
	$(M68K_STRIP) -s $(NETBSD) -o $(NETBSD).stripped
	gzip -9 $(NETBSD).stripped
	mv $(NETBSD).stripped.gz netbsd.gz
endif

export SIGNATURE VERSION DESTDIR PREFIX KARCH CROSS_COMPILE

first::
	$(MAKE) -C first OBJCOPY=$(M68K_OBJCOPY) AS=$(M68K_AS)

second/$(KARCH)-linux-floppy/second:: libmacos libunix libiso9660-m68k libgzip-m68k libfloppy libscsi libstream libblock
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) MEDIA=floppy TARGET=$(KARCH)-linux

second/$(KARCH)-linux-scsi/second:: libmacos libunix libiso9660-m68k libgzip-m68k libscsi libstream libblock
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) TARGET=$(KARCH)-linux MEDIA=scsi

second/m68k-netbsd-floppy/second:: libmacos libunix libiso9660-m68k libgzip-m68k libfloppy libstream libblock
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

docs::
	$(MAKE) -C docs all

dump: last.bin
	dd if=last.bin of=$(FLOPPY) bs=512

install: tools-install first-install second-install docs-install

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

clean:: libemile-clean libmacos-clean libunix-clean tools-clean first-clean \
	second-clean docs-clean libiso9660-clean libgzip-clean libfloppy-clean \
	libscsi-clean libstream-clean libblock-clean
	rm -f floppy.bin floppy.bin.X floppy_ramdisk.bin \
	      floppy_ramdisk.bin.X rescue.bin rescue.bin.X \
	      debian-installer.bin debian-installer.bin.X \
	      netboot.bin netboot.bin.X boot.bin boot.bin.X \
	      vmlinuz last.bin

DISTFILES = AUTHORS ChangeLog COPYING Makefile README README.floppy \
	    README.scsi

dist:
	rm -fr $(PACKAGE)-$(VERSION)
	mkdir $(PACKAGE)-$(VERSION)
	$(MAKE) -C tools dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	$(MAKE) -C libemile dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	$(MAKE) -C second dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	$(MAKE) -C first dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	$(MAKE) -C docs dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	$(MAKE) -C libmacos dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	$(MAKE) -C libunix dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
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
