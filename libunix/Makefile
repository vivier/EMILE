#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic
CPPFLAGS = -I$(TOP) -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libunix.a

SOURCES = divsi3.S modsi3.S mulsi3.S udivsi3.S umodsi3.S free.c malloc.c \
	  memcpy.c memset.c printf.c putchar.c puts.c read.c sprintf.c \
	  strcpy.c strlen.c strncpy.c vsprintf.c write.c

HEADERS = 

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
			mkdir -p $(DISTDIR)/libunix/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/libunix/$$file; \
	done

clean:
	rm -f $(OBJS) $(LIBRARY)
