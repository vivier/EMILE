#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP=$(shell pwd)

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libfloppy.a

SOURCES = floppy_read_sector.c floppy_close.c floppy_open.c

HEADERS = libfloppy.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
