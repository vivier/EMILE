#
#  (c) 2005 Laurent Vivier <Laurent@lvivier.info>
#

TOP=$(shell pwd)

68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K -I$(TOP)/../libstream

LIBRARY = libblock.a

SOURCES = block_close.c block_fstat.c block_lseek.c block_open.c block_read.c

HEADERS = libblock.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
