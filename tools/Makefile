#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar

PROGRAMS = emile-set-cmdline emile-first-tune emile-install \
	   emile-set-output emile emile-map-set iso9660_ls \
	   iso9660_cat minigzip read_vmlinuz

SOURCES	= emile-set-cmdline.c Makefile emile-first-tune.c \
		  emile-install.c emile-set-output.c emile.c \
		  emile_scanbus.c emile-map-set.c iso9660_ls.c \
		  iso9660_cat.c minigzip.c read_vmlinuz.c

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS) -DSIGNATURE="\"$(SIGNATURE)\"" -DPREFIX=\"$(PREFIX)\" \
	   -I../libemile -I../libiso9660 -I../libgzip -I../libstream \
	   -DFATFREE -DNO_GZCOMPRESS

CFLAGS	= -Wall -g
LDLIBS = $(CROSS_COMPILE_LDFLAGS) -L../libemile -lemile -L../libiso9660/native -liso9660 -L../libgzip/native -lgzip

all: $(PROGRAMS)

emile: emile.o emile_scanbus.o

emile-map-set: emile-map-set.o emile_scanbus.o

iso9660_ls: iso9660_ls.o device.o

iso9660_cat: iso9660_cat.o device.o

minigzip: minigzip.c gzio.c

read_vmlinuz: read_vmlinuz.o gzio.o

install:
	install -d $(DESTDIR)/$(PREFIX)/sbin/
	install emile-set-cmdline $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	install emile-first-tune $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	install emile-install $(DESTDIR)/$(PREFIX)/sbin/emile-install
	install emile-set-output $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	install emile $(DESTDIR)/$(PREFIX)/sbin/emile
	install emile-map-set $(DESTDIR)/$(PREFIX)/sbin/emile-map-set

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-set-cmdline
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-first-tune
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-install
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-set-output
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile
	rm -f $(DESTDIR)/$(PREFIX)/sbin/emile-map-set

dist:
	for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/tools/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/tools/$$file; \
	done

clean:
	rm -f *.o $(PROGRAMS)
