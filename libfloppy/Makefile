#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP=$(shell pwd)
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libfloppy.a

SOURCES = floppy_lseek.c floppy_read.c floppy_read_sector.c

HEADERS = libfloppy.h

DISTFILES = $(SOURCES) $(HEADERS)

OBJS	= $(patsubst %.S,%.o,$(SOURCES:.c=.o))

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rc $@ $^

dist:
	for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/libfloppy/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/libfloppy/$$file; \
	done

clean:
	rm -f $(OBJS) $(LIBRARY)
