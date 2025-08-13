#
#
# (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
#
#

TOP=$(shell pwd)

PROGRAMS = emile-set-cmdline emile-first-tune emile-install \
	   emile-set-output emile emile-map-set iso9660_ls \
	   iso9660_cat minigzip read_vmlinuz emile-mkisofs \
	   ext2_ls ext2_cat emile-mktable emile-conf

SOURCES	= emile-set-cmdline.c emile-first-tune.c \
		  emile-install.c emile-set-output.c emile.c \
		  emile_scanbus.c emile-map-set.c iso9660_ls.c \
		  iso9660_cat.c minigzip.c read_vmlinuz.c device.c gzio.c \
		  emile-mkisofs.c ext2_ls.c ext2_cat.c emile-mktable.c \
		  emile-conf.c

HEADERS	= device.h

DISTFILES =$(HEADERS) $(SOURCES) Makefile

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS) -DSIGNATURE="\"$(SIGNATURE)\"" -DPREFIX=\"$(PREFIX)\" \
	   -I$(TOP)/../libemile -I$(TOP)/../libiso9660 -I$(TOP)/../libgzip -I$(TOP)/../libstream \
	   -I$(TOP)/../libcontainer -I$(TOP)/../libmacos -DFATFREE -DNO_GZCOMPRESS \
	   -I$(TOP)/../libconfig -I$(TOP)/../libmap -I$(TOP)/../libext2

CFLAGS	= -Wall -Werror -g -Wno-address-of-packed-member
LDLIBS = $(CROSS_COMPILE_LDFLAGS) -L$(TOP)/../libemile/$(TARGET) -lemile -L$(TOP)/../libiso9660/$(TARGET) -liso9660 -L$(TOP)/../libgzip/$(TARGET) -lgzip -L$(TOP)/../libconfig/$(TARGET) -lconfig -L$(TOP)/../libmap/$(TARGET) -lmap -L$(TOP)/../libext2/$(TARGET) -lext2

all:
	test -d $(TARGET) || mkdir $(TARGET)
		$(MAKE) -C $(TARGET) -f $(TOP)/Makefile $(PROGRAMS) TOP=$(TOP) VPATH=$(TOP)


emile-install: emile-install.o

emile: emile.o emile_scanbus.o device.o

emile-map-set: emile-map-set.o emile_scanbus.o device.o

iso9660_ls: iso9660_ls.o device.o

iso9660_cat: iso9660_cat.o device.o

ext2_ls: ext2_ls.o device.o

ext2_cat: ext2_cat.o device.o

minigzip: minigzip.c gzio.c

read_vmlinuz: read_vmlinuz.o gzio.o

emile-mkisofs: emile-mkisofs.o device.o
emile-mktable: emile-mktable.o device.o
emile-conf: emile-conf.o device.o

install:
	install -d $(DESTDIR)/$(PREFIX)/sbin/
	install $(TARGET)/emile-set-cmdline $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	install $(TARGET)/emile-first-tune $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	install $(TARGET)/emile-install $(DESTDIR)/$(PREFIX)/sbin/emile-install
	install $(TARGET)/emile-set-output $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	install $(TARGET)/emile $(DESTDIR)/$(PREFIX)/sbin/emile
	install $(TARGET)/emile-map-set $(DESTDIR)/$(PREFIX)/sbin/emile-map-set
	install $(TARGET)/emile-mkisofs $(DESTDIR)/$(PREFIX)/sbin/emile-mkisofs
	install $(TARGET)/emile-mktable $(DESTDIR)/$(PREFIX)/sbin/emile-mktable
	install $(TARGET)/emile-conf $(DESTDIR)/$(PREFIX)/sbin/emile-conf

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-install
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-map-set
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-mkisofs
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-mktable
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-conf

include $(TOP)/../config.mk
include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
