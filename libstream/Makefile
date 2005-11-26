#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)
VPATH=$(TOP)

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -g -O2 -m68000 -Wa,-m68000 -Os
CPPFLAGS = -I$(TOP)/../libmacos -I$(TOP) -I$(TOP)/../libgzip

LIBRARIES = hard/libstream.a floppy/libstream.a

SOURCES = stream_close.c stream_open.c stream_read.c stream_lseek.c \
	  stream_uncompress.c gzio.c stream_fstat.c

HEADERS = libstream.h

DISTFILES = $(SOURCES) $(HEADERS)

all: $(LIBRARIES)

hard/libstream.a::
	test -e hard || mkdir hard
	$(MAKE) -C hard -f $(TOP)/Makefile TOP=$(TOP) LIBRARY=libstream.a CPPFLAGS="$(CPPFLAGS) -DSCSI_SUPPORT -DISO9660_SUPPORT -I$(TOP)/../libscsi -I$(TOP)/../libiso9660" libstream.a

floppy/libstream.a::
	test -e floppy || mkdir floppy
	$(MAKE) -C floppy -f $(TOP)/Makefile TOP=$(TOP) LIBRARY=libstream.a CPPFLAGS="$(CPPFLAGS) -DFLOPPY_SUPPORT -DBLOCK_SUPPORT -I$(TOP)/../libfloppy -I$(TOP)/../libblock" libstream.a

include $(TOP)/../Rules.mk
