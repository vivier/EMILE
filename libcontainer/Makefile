#
#  (c) 2005 Laurent Vivier <Laurent@lvivier.info>
#

TOP=$(shell pwd)
VPATH=$(TOP)

68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP)/../libstream

LIBRARY = libcontainer.a

SOURCES = container_close.c container_lseek.c container_open.c \
	  container_read.c container_fstat.c

HEADERS = libcontainer.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
