#
#
# (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
#
#

TOP=$(shell pwd)

PROGRAMS = emile-set-cmdline emile-first-tune emile-install \
	   emile-set-output emile emile-map-set iso9660_ls \
	   iso9660_cat minigzip read_vmlinuz emile-mkisofs

SOURCES	= emile-set-cmdline.c Makefile emile-first-tune.c \
		  emile-install.c emile-set-output.c emile.c \
		  emile_scanbus.c emile-map-set.c iso9660_ls.c \
		  iso9660_cat.c minigzip.c read_vmlinuz.c device.c gzio.c \
		  emile-mkisofs.c emile_config.c

HEADERS	= device.h emile_config.h

DISTFILES =$(HEADERS) $(SOURCES) Makefile

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS) -DSIGNATURE="\"$(SIGNATURE)\"" -DPREFIX=\"$(PREFIX)\" \
	   -I../libemile -I../libiso9660 -I../libgzip -I../libstream \
	   -I../libcontainer -I../libmacos -DFATFREE -DNO_GZCOMPRESS \
	   -I../libconfig -I../libmap

CFLAGS	= -Wall -Werror -g
LDLIBS = $(CROSS_COMPILE_LDFLAGS) -L../libemile -lemile -L../libiso9660/native -liso9660 -L../libgzip/native -lgzip -L../libconfig/native -lconfig -L../libmap/native -lmap

all: $(PROGRAMS)

emile-install: emile-install.o emile_config.o

emile: emile.o emile_scanbus.o emile_config.o device.o

emile-map-set: emile-map-set.o emile_scanbus.o device.o

iso9660_ls: iso9660_ls.o device.o

iso9660_cat: iso9660_cat.o device.o

minigzip: minigzip.c gzio.c

read_vmlinuz: read_vmlinuz.o gzio.o

emile-mkisofs: emile-mkisofs.o device.o

install:
	install -d $(DESTDIR)/$(PREFIX)/sbin/
	install emile-set-cmdline $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	install emile-first-tune $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	install emile-install $(DESTDIR)/$(PREFIX)/sbin/emile-install
	install emile-set-output $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	install emile $(DESTDIR)/$(PREFIX)/sbin/emile
	install emile-map-set $(DESTDIR)/$(PREFIX)/sbin/emile-map-set
	install emile-mkisofs $(DESTDIR)/$(PREFIX)/sbin/emile-mkisofs

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-install
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-map-set
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-mkisofs

dist:
	@echo TAR tools
	@for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/tools/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/tools/$$file; \
	done

clean:
	rm -f *.o $(PROGRAMS)

include $(TOP)/../config.mk
include $(TOP)/../tools.mk
