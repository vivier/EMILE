#
#  (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
#

TOP = $(shell pwd)
VPATH=$(TOP)

TARGET = native
CFLAGS	+= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar
ifeq ($(TARGET), m68k-linux)
68000FLAGS = -m68000 -Wa,-m68000
CFLAGS += -fpic -O2 -Os
else
CFLAGS += -g
endif
CPPFLAGS = -I$(TOP)/../libstream

LIBRARY = libiso9660.a

SOURCES = iso9660_mount.c iso9660_opendir.c \
	  iso9660_closedir.c iso9660_readdir.c \
	  iso9660_is_directory.c iso9660_open.c \
	  iso9660_read.c iso9660_close.c \
	  iso9660_lseek.c iso9660_fstat.c

HEADERS = libiso9660.h

all:
	test -d $(TARGET) || mkdir $(TARGET)
	$(MAKE) -C $(TARGET) -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
