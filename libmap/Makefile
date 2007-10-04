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
endif

LIBRARY = libmap.a

SOURCES = map_bootblock_get_type.c map_bootblock_is_valid.c \
	  map_bootblock_read.c map_bootblock_write.c map_close.c map_dev.c \
	  map_geometry.c map_get_bootinfo.c map_get_driver_info.c \
	  map_get_driver_number.c map_get_driver_signature.c map_get_number.c \
	  map_get_partition_geometry.c map_get_partition_name.c \
	  map_get_partition_type.c map_has_apple_driver.c map_is_valid.c \
	  map_open.c map_partition_get_flags.c map_partition_is_bootable.c \
	  map_partition_is_startup.c map_partition_is_valid.c \
	  map_partition_set_bootable.c map_partition_set_flags.c \
	  map_partition_set_startup.c map_read.c map_seek_driver_partition.c \
	  map_set_bootinfo.c map_set_driver_info.c map_set_driver_number.c \
	  map_set_partition_name.c map_set_partition_type.c map_set_startup.c \
	  map_write.c map_block0_write.c

HEADERS = libmap.h

all:
	test -d $(TARGET) || mkdir $(TARGET)
	cd $(TARGET) && make -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
