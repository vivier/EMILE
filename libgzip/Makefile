#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)

VPATH=$(TOP)

TARGET = native

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic
CPPFLAGS = -DFATFREE -DNOBYFOUR -DBUILDFIXED -DNO_GZCOMPRESS -DDYNAMIC_CRC_TABLE

LIBRARY = libgzip.a

SOURCES = inflate.c adler32.c inftrees.c zutil.c inffast.c crc32.c

HEADERS = deflate.h inffast.h inflate.h inftrees.h zconf.h zlib.h zutil.h

all:
	test -d $(TARGET) || mkdir $(TARGET)
	cd $(TARGET) && make -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)

include $(TOP)/../Rules.mk
