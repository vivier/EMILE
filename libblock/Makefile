#
#  (c) 2005-2007 Laurent Vivier <Laurent@Vivier.EU>
#

TOP=$(shell pwd)

68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	+= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K -I$(TOP)/../libstream

LIBRARY = libblock.a

SOURCES = block_close.c block_fstat.c block_lseek.c block_open.c block_read.c \
	  block_init.c block_mount.c

HEADERS = libblock.h block.h

all: $(LIBRARY)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
