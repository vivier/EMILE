#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)
VPATH=$(TOP)

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -g
CPPFLAGS = -I$(TOP)/../libfloppy -I$(TOP)/../libscsi -I$(TOP)/../libblock \
	   -I$(TOP)/../libblock -I$(TOP)/../libiso9660 -I$(TOP)/../libmacos \
	   -I$(TOP)/../libcontainer -I$(TOP)

LIBRARY = libstream.a

SOURCES = stream_close.c stream_open.c stream_read.c stream_lseek.c

HEADERS = libstream.h

DISTFILES = $(SOURCES) $(HEADERS)

all: $(LIBRARY)

include $(TOP)/../Rules.mk
