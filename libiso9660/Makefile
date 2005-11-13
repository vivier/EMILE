#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)
TARGET = native
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic
CPPFLAGS = -I$(TOP) -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libiso9660.a

SOURCES = iso9660_mount.c iso9660_opendir.c \
	  iso9660_closedir.c iso9660_readdir.c \
	  iso9660_is_directory.c iso9660_open.c \
	  iso9660_read.c iso9660_close.c iso9660_init.c

HEADERS = libiso9660.a

DISTFILES = $(SOURCES) $(HEADERS)

OBJS	= $(patsubst %.S,%.o,$(SOURCES:.c=.o))

all:
	test -d $(TARGET) || mkdir $(TARGET)
	cd $(TARGET) && make -f $(TOP)/Makefile $(LIBRARY) VPATH=$(TOP)

$(LIBRARY): $(OBJS)
	$(AR) rc $@ $^

install: install-headers install-lib

install-lib:
	install -d $(DESTDIR)/$(PREFIX)/usr/lib/
	install $(TARGET)/$(LIBRARY) $(DESTDIR)/$(PREFIX)/usr/lib/$(LIBRARY)

install-headers:
	for file in $(HEADERS); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DESTDIR)/$(PREFIX)/usr/include/$$dir; \
		fi; \
		cp -p $$file $(DESTDIR)/$(PREFIX)/usr/include/$$file; \
	done

uninstall: uninstall-lib uninstall-headers

uninstall-lib:
	rm -f $(DESTDIR)/$(PREFIX)/usr/lib/$(LIBRARY)

uninstall-headers:
	for file in $(HEADERS); do \
		rm -f $(DESTDIR)/$(PREFIX)/usr/include/$$file; \
	done

dist:
	for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/libunix/$$dir; \
		fi; \
		cp -p $(TARGET)/$$file $(DISTDIR)/libunix/$$file; \
	done

clean:
	cd $(TARGET) && rm -f $(OBJS) $(LIBRARY)
