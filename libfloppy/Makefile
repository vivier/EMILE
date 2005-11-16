#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP=$(shell pwd)

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libfloppy.a

SOURCES = floppy_lseek.c floppy_read.c floppy_read_sector.c

HEADERS = libfloppy.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
