#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP	= $(shell pwd)
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -m68000 -Wa,-m68000 -Os
CPPFLAGS	= -I$(TOP) -DARCH_M68K

LIBRARY = libmacos.a

SOURCES = display_properties.S \
	  CloseDriver.c OpenDriver.c SerGetBuf.c

HEADERS = libmacos.h \
	  macos/devices.h macos/errors.h macos/events.h macos/files.h \
	  macos/gestalt.h macos/header macos/init.h macos/lowmem.h \
	  macos/memory.h macos/osutils.h macos/quickdraw.h macos/romdefs.h \
	  macos/scsi.h macos/serial.h macos/slotmanager.h macos/traps.h \
	  macos/types.h macos/video.h

DISTFILES = $(SOURCES) $(HEADERS)

OBJS	= $(patsubst %.S,%.o,$(SOURCES:.c=.o))

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rc $@ $^

install: install-headers install-lib

install-lib:
	install -d $(DESTDIR)/$(PREFIX)/usr/lib/
	install $(LIBRARY) $(DESTDIR)/$(PREFIX)/usr/lib/$(LIBRARY)

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
			mkdir -p $(DISTDIR)/libmacos/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/libmacos/$$file; \
	done

clean:
	rm -f $(OBJS) $(LIBRARY)
