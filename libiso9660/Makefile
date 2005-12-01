#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)
VPATH=$(TOP)

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar

TARGET = native
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar
ifeq ($(TARGET), m68k-linux)
CFLAGS += -fpic -O2 -m68000 -Wa,-m68000 -Os
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

include $(TOP)/../Rules.mk
