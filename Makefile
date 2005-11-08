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
RESCUE_ARGS="root=/dev/ramdisk $(CONSOLE)"
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

all: libemile tools first/first_floppy second/$(KARCH)-linux-floppy/second \
     second/$(KARCH)-linux-scsi/second second/m68k-netbsd-floppy/second

# We can build floppy image only if a kernel is provided

ifeq ($(LINUX),$(LINUXPATH))
all_bin: netboot.bin rescue.bin debian-installer.bin boot.bin
	rm -f last.bin

floppy.bin: tools first/first_floppy vmlinuz \
	    second/$(KARCH)-linux-floppy/second
	tools/emile-install -f first/first_floppy \
			    -s second/$(KARCH)-linux-floppy/second \
			    -k vmlinuz floppy.bin.X
ifdef CONSOLE
	tools/emile-set-output floppy.bin.X --printer --modem
endif
	mv floppy.bin.X floppy.bin

floppy_ramdisk.bin: tools first/first_floppy vmlinuz \
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

vmlinux.bin: $(LINUX)
	$(M68K_OBJCOPY) -I elf32-big -O binary -R .note -R .comment -S $(LINUX) vmlinux.bin

vmlinuz: vmlinux.bin
	cp vmlinux.bin vmlinuz.out
	gzip -9 vmlinuz.out
	mv vmlinuz.out.gz vmlinuz
endif

NETBSDPATH=netbsd

NETBSD=$(shell ls $(NETBSDPATH) 2> /dev/null)

ifeq ($(NETBSD),$(NETBSDPATH))
netbsd-floppy.bin: tools first/first_floppy netbsd.gz \
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

netbsd.bin: $(LINUX)
	$(M68K_OBJCOPY) -I elf32-big -O binary -R .note -R .comment -S $(NETBSD) netbsd.bin

netbsd.gz: netbsd.bin
	cp netbsd.bin netbsd.out
	gzip -9 netbsd.out
	mv netbsd.out.gz netbsd.gz
endif

first/first_floppy::
	$(MAKE) -C first OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) AS=$(M68K_AS) SIGNATURE="$(SIGNATURE)"

second/$(KARCH)-linux-floppy/second:: libmacos libunix
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
		TARGET=$(KARCH)-linux MEDIA=floppy

second/$(KARCH)-linux-scsi/second:: libmacos libunix
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
		TARGET=$(KARCH)-linux MEDIA=scsi

second/m68k-netbsd-floppy/second:: libmacos libunix
	$(MAKE) -C second OBJCOPY=$(M68K_OBJCOPY) LD=$(M68K_LD) CC=$(M68K_CC) \
		AS=$(M68K_AS) VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
		TARGET=m68k-netbsd MEDIA=floppy

first-install:: first/first_floppy
	$(MAKE) -C first install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

first-uninstall::
	$(MAKE) -C first uninstall DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

second-install:: second/$(KARCH)-floppy/second
	$(MAKE) -C first install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) \
				 KARCH=$(KARCH)

second-uninstall::
	$(MAKE) -C first uninstall DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) \
				   KARCH=$(KARCH)

libmacos::
	$(MAKE) -C libmacos all CC=$(M68K_CC) AS=$(M68K_AS)

libmacos-install:: libmacos
	$(MAKE) -C libmacos install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

libmacos-uninstall::
	$(MAKE) -C libmacos uninstall DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

libunix::
	$(MAKE) -C libunix all CC=$(M68K_CC) AS=$(M68K_AS)

libemile::
	$(MAKE) -C libemile all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
		CROSS_COMPILE=$(CROSS_COMPILE)

libemile-install:: libemile
	$(MAKE) -C libemile install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

libemile-uninstall::
	$(MAKE) -C libemile uninstall DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

tools:: libemile
	$(MAKE) -C tools all VERSION=$(VERSION) SIGNATURE="$(SIGNATURE)" \
			     PREFIX=$(PREFIX) CROSS_COMPILE=$(CROSS_COMPILE)

tools-install:: tools
	$(MAKE) -C tools install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

tools-uninstall::
	$(MAKE) -C tools uninstall DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

docs-install:: docs
	$(MAKE) -C docs install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

docs-uninstall::
	$(MAKE) -C docs uninstall DESTDIR=$(DESTDIR) PREFIX=$(PREFIX)

docs::
	$(MAKE) -C docs all

dump: last.bin
	dd if=last.bin of=$(FLOPPY) bs=512
	# eject makes hanging my USB floppy device
	#eject $(FLOPPY)

install: libmacos-install libemile-install tools-install first-install \
	 docs-install

uninstall: libmacos-uninstall libemile-uninstall tools-uninstall \
	   first-uninstall docs-uninstall

libemile-clean:
	$(MAKE) -C libemile clean

libmacos-clean:
	$(MAKE) -C libmacos clean

libunix-clean:
	$(MAKE) -C libunix clean

tools-clean:
	$(MAKE) -C tools clean

first-clean:
	$(MAKE) -C first clean

second-clean:
	$(MAKE) -C second clean

docs-clean:
	$(MAKE) -C docs clean

clean:: libemile-clean libmacos-clean libunix-clean tools-clean first-clean \
	second-clean docs-clean
	rm -f floppy.bin floppy.bin.X floppy_ramdisk.bin \
	      floppy_ramdisk.bin.X rescue.bin rescue.bin.X \
	      debian-installer.bin debian-installer.bin.X \
	      netboot.bin netboot.bin.X boot.bin boot.bin.X \
	      vmlinuz vmlinux.bin last.bin

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
