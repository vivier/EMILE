#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP=$(shell pwd)

68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libfloppy.a

SOURCES = floppy_read_sector.c floppy_close.c floppy_open.c \
	  floppy_get_blocksize.c

HEADERS = libfloppy.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
