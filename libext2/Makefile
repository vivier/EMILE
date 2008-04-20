#
#  (c) 2008 Laurent Vivier <Laurent@lvivier.info>
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

LIBRARY = libext2.a

SOURCES = ext2_mount.c ext2_opendir.c \
	  ext2_closedir.c \
	  ext2_open.c ext2_init.c \
	  ext2_read.c ext2_close.c \
	  ext2_lseek.c ext2_fstat.c \
	  ext2_utils.c ext2_readdir.c

HEADERS = libext2.h ext2_utils.h ext2.h

all:
	test -d $(TARGET) || mkdir $(TARGET)
	$(MAKE) -C $(TARGET) -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
