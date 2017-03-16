#
#  (c) 2004-2013 Laurent Vivier <Laurent@Vivier.EU>
#
#

MAKEFLAGS += --no-print-directory

include config.mk
include tools.mk
include kernel.mk

SYSTEM:=$(shell uname -s)
MACHINE:=$(shell uname -m)
ifeq ($(SYSTEM), Linux)
NATIVE:=$(MACHINE)-linux
else
NATIVE:=$(MACHINE)-$(SYSTEM)
endif

# Target

.PHONY: first libemile libblock libiso9660 libiso9660-m68k libgzip-m68k tools \
       libcontainer clean all_bin all install tools-install first-install \
       docs-install uninstall tools-uninstall first-uninstall docs-uninstall \
       clean libemile-clean libmacos-clean libunix-clean tools-clean \
       first-clean second-clean docs-clean libiso9660-clean libgzip-clean \
       libfloppy-clean libscsi-clean libstream-clean libblock-clean dist docs \
       libconfig libconfig-m68k libmap \
       libmap-m68k libext2 libext2-m68k

all: tools.mk docs libemile libblock libiso9660 libiso9660-m68k libgzip-m68k \
     tools first libstream libcontainer libmap libext2 libext2-m68k \
     second/$(KARCH)-linux-floppy/second \
     second/$(KARCH)-linux-scsi/second second/m68k-netbsd-floppy/second \
     libconfig libconfig-m68k \
     second/$(KARCH)-linux-all/second second/m68k-linux-scsi-driver/apple_driver

tools.mk: scripts/tools.sh
	sh scripts/tools.sh > $@

ALL_BIN = cdboot-sarge.bin cdboot-woody.bin cdboot-etch.bin

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

cdboot-etch.bin: tools first second/$(KARCH)-linux-scsi/second
	tools/emile-install -f first/first_floppy \
			-s second/$(KARCH)-linux-scsi/second \
			-k "iso9660:(sd3)/install/kernels/vmlinuz-2.6.18-4-mac"\
			-r "iso9660:(sd3)/install/cdrom/initrd.gz" \
			    cdboot-etch.bin.X
	tools/emile-set-cmdline -a $(CDBOOT_ARGS26) cdboot-etch.bin.X
	mv cdboot-etch.bin.X cdboot-etch.bin
	rm -f last.bin
	ln -s cdboot-etch.bin last.bin

# We can build floppy image only if a kernel is provided

ifeq ($(LINUX),$(LINUXPATH))

ALL_BIN += netboot.bin rescue.bin debian-installer.bin boot.bin multiboot.bin second/$(KARCH)-linux-scsi/second second/$(KARCH)-linux-all/second

floppy.bin: tools first vmlinuz \
	    second/$(KARCH)-linux-floppy/second
	tools/emile-install -f first/first_floppy \
			    -s second/$(KARCH)-linux-floppy/second \
			    -k vmlinuz floppy.bin.X
ifdef CONSOLE
	tools/emile-set-output floppy.bin.X --printer --modem
endif
ifdef GESTALTID
	tools/emile-set-output floppy.bin.X --gestaltid $(GESTALTID)
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
ifdef GESTALTID
	tools/emile-set-output floppy_ramdisk.bin.X --gestaltid $(GESTALTID)
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

multiboot.bin: tools first vmlinuz second/$(KARCH)-linux-all/second
	rm -f last.bin
	tools/emile-install -c $(FLOPPY_CONF) multiboot.bin.X
	mv multiboot.bin.X multiboot.bin
	ln -s multiboot.bin last.bin

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

all_bin: $(ALL_BIN)
	rm -f last.bin

export SIGNATURE VERSION DESTDIR PREFIX KARCH CROSS_COMPILE

first::
	$(MAKE) -C first TARGET=m68k-linux

second/$(KARCH)-linux-floppy/second:: libmacos libunix libiso9660-m68k libext2-m68k libgzip-m68k libfloppy libscsi libstream libblock libcontainer libui libconfig-m68k libmap-m68k
	$(MAKE) -C second MEDIA=floppy TARGET=$(KARCH)-linux

second/$(KARCH)-linux-scsi/second:: libmacos libunix libiso9660-m68k libext2-m68k libgzip-m68k libfloppy libscsi libstream libblock libcontainer libui libconfig-m68k libmap-m68k
	$(MAKE) -C second MEDIA=scsi TARGET=$(KARCH)-linux

second/$(KARCH)-linux-all/second:: libmacos libunix libiso9660-m68k libext2-m68k libgzip-m68k libfloppy libscsi libstream libblock libcontainer libui libconfig-m68k libmap-m68k
	$(MAKE) -C second MEDIA=full TARGET=$(KARCH)-linux

second/m68k-netbsd-floppy/second:: libmacos libunix libiso9660-m68k libext2-m68k libgzip-m68k libfloppy libstream libblock libcontainer libui libconfig-m68k libmap-m68k
	$(MAKE) -C second TARGET=m68k-netbsd MEDIA=floppy

second/m68k-linux-scsi-driver/apple_driver:: libmacos libunix libiso9660-m68k libgzip-m68k libstream libui libconfig-m68k libmap-m68k
	$(MAKE) -C second MEDIA=scsi TARGET=m68k-linux \
	        m68k-linux-scsi-driver/apple_driver

first-install::
	$(MAKE) -C first install

first-uninstall::
	$(MAKE) -C first uninstall

second-install::
	$(MAKE) -C second install

second-uninstall::
	$(MAKE) -C second uninstall

libmacos::
	$(MAKE) -C libmacos all TARGET=m68k-linux

libunix::
	$(MAKE) -C libunix all TARGET=m68k-linux

libui::
	$(MAKE) -C libui all TARGET=m68k-linux

libiso9660-m68k::
	$(MAKE) -C libiso9660 all TARGET=m68k-linux

libext2-m68k::
	$(MAKE) -C libext2 all TARGET=m68k-linux

libconfig-m68k::
	$(MAKE) -C libconfig all TARGET=m68k-linux

libconfig::
	$(MAKE) -C libconfig all TARGET=$(NATIVE) CROSS_COMPILE=$(CROSS_COMPILE)

libiso9660::
	$(MAKE) -C libiso9660 all TARGET=$(NATIVE) CROSS_COMPILE=$(CROSS_COMPILE)

libext2::
	$(MAKE) -C libext2 all TARGET=$(NATIVE) CROSS_COMPILE=$(CROSS_COMPILE)

libcontainer::
	$(MAKE) -C libcontainer all TARGET=m68k-linux

libblock::
	$(MAKE) -C libblock all TARGET=m68k-linux

libgzip-m68k::
	$(MAKE) -C libgzip all TARGET=m68k-linux

libgzip::
	$(MAKE) -C libgzip all TARGET=$(NATIVE) CROSS_COMPILE=$(CROSS_COMPILE)

libemile::
	$(MAKE) -C libemile all TARGET=$(NATIVE) CROSS_COMPILE=$(CROSS_COMPILE)

libfloppy::
	$(MAKE) -C libfloppy all CC=$(M68K_CC) AS=$(M68K_AS)

libscsi::
	$(MAKE) -C libscsi all CC=$(M68K_CC) AS=$(M68K_AS)

libstream::
	$(MAKE) -C libstream all CC=$(M68K_CC) AS=$(M68K_AS)

libmap::
	$(MAKE) -C libmap all TARGET=$(NATIVE) CROSS_COMPILE=$(CROSS_COMPILE)

libmap-m68k::
	$(MAKE) -C libmap all TARGET=m68k-linux

tools::  libemile libiso9660 libext2 libgzip libconfig libmap
	$(MAKE) -C tools all CROSS_COMPILE=$(CROSS_COMPILE) TARGET=$(NATIVE)

tools-install:: tools
	$(MAKE) -C tools install TARGET=$(NATIVE)

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

libui-clean:
	$(MAKE) -C libui clean

libscsi-clean:
	$(MAKE) -C libscsi clean

libstream-clean:
	$(MAKE) -C libstream clean

libiso9660-clean::
	$(MAKE) -C libiso9660 clean TARGET=$(NATIVE)
	$(MAKE) -C libiso9660 clean TARGET=$(KARCH)-linux

libext2-clean::
	$(MAKE) -C libext2 clean TARGET=$(NATIVE)
	$(MAKE) -C libext2 clean TARGET=$(KARCH)-linux

libconfig-clean::
	$(MAKE) -C libconfig clean TARGET=$(NATIVE)
	$(MAKE) -C libconfig clean TARGET=$(KARCH)-linux

libcontainer-clean::
	$(MAKE) -C libcontainer clean

libblock-clean::
	$(MAKE) -C libblock clean

libgzip-clean::
	$(MAKE) -C libgzip clean TARGET=$(NATIVE)
	$(MAKE) -C libgzip clean TARGET=$(KARCH)-linux

libmap-clean::
	$(MAKE) -C libmap clean TARGET=$(NATIVE)
	$(MAKE) -C libmap clean TARGET=$(KARCH)-linux

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

distclean:: clean
	rm -f tools.mk

clean:: libemile-clean libmacos-clean libunix-clean tools-clean first-clean \
	second-clean docs-clean libiso9660-clean libext2-clean libgzip-clean \
	libfloppy-clean \
	libscsi-clean libstream-clean libblock-clean libcontainer-clean \
	libui-clean libconfig-clean libmap-clean
	rm -f floppy.bin floppy.bin.X floppy_ramdisk.bin \
	      floppy_ramdisk.bin.X rescue.bin rescue.bin.X \
	      debian-installer.bin debian-installer.bin.X \
	      netboot.bin netboot.bin.X boot.bin boot.bin.X \
	      vmlinuz last.bin cdboot-sarge.bin cdboot-woody.bin \
	      multiboot.bin

DISTFILES = AUTHORS ChangeLog COPYING Makefile README README.floppy \
	    README.scsi Rules.mk config/floppy.conf kernel.mk config.mk \
	    scripts/tools.sh

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
	@$(MAKE) -C libui dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libstream dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libscsi dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libfloppy dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libblock dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libiso9660 dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libext2 dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libcontainer dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libgzip dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C tools dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C debian dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libconfig dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
	@$(MAKE) -C libmap dist DISTDIR=$(shell pwd)/$(PACKAGE)-$(VERSION)
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

ChangeLog::
	cvs2cl -U CVS/users --no-common-dir --accum
