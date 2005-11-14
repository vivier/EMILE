#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)
TARGET = native
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic
CPPFLAGS = -DFATFREE -DNOBYFOUR -DBUILDFIXED -DNO_GZCOMPRESS -DDYNAMIC_CRC_TABLE

LIBRARY = libgzip.a

SOURCES = inflate.c adler32.c inftrees.c zutil.c inffast.c crc32.c

HEADERS = deflate.h inffast.h inflate.h inftrees.h zconf.h zlib.h zutil.h

DISTFILES = $(SOURCES) $(HEADERS)

OBJS	= $(patsubst %.S,%.o,$(SOURCES:.c=.o))

all:
	test -d $(TARGET) || mkdir $(TARGET)
	cd $(TARGET) && make -f $(TOP)/Makefile $(LIBRARY) VPATH=$(TOP)

$(LIBRARY): $(OBJS)
	$(AR) rc $@ $^

dist:
	for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/libgzip/$$dir; \
		fi; \
		cp -p $(TARGET)/$$file $(DISTDIR)/libgzip/$$file; \
	done

clean:
	cd $(TARGET) && rm -f $(OBJS) $(LIBRARY)
