#
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

TOP=$(shell pwd)

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS)
LDFLAGS = $(CROSS_COMPILE_LDFLAGS)

LIBRARY = libemile.a

SOURCES = emile_block0_write.c emile_checksum.c emile_first_get_param.c \
	  emile_first_set_param.c emile_first_set_param_scsi.c \
	  emile_floppy_create_image.c \
	  emile_map_bootblock_get_type.c emile_map_bootblock_is_valid.c \
	  emile_map_bootblock_read.c emile_map_bootblock_write.c \
	  emile_map_close.c emile_map_dev.c emile_map_geometry.c \
	  emile_map_get_bootinfo.c emile_map_get_driver_info.c \
	  emile_map_get_driver_number.c emile_map_get_number.c \
	  emile_map_get_partition_geometry.c emile_map_get_partition_name.c \
	  emile_map_get_partition_type.c emile_map_has_apple_driver.c \
	  emile_map_is_valid.c emile_map_open.c \
	  emile_map_partition_get_flags.c emile_map_partition_is_bootable.c \
	  emile_map_partition_is_startup.c emile_map_partition_is_valid.c \
	  emile_map_partition_set_bootable.c emile_map_partition_set_flags.c \
	  emile_map_partition_set_startup.c emile_map_read.c \
	  emile_map_seek_driver_partition.c emile_map_set_bootinfo.c \
	  emile_map_set_driver_info.c emile_map_set_driver_number.c \
	  emile_map_set_partition_name.c emile_map_set_partition_type.c \
	  emile_map_set_startup.c emile_map_write.c \
	  emile_scsi_create_container.c emile_scsi_get_dev.c \
	  emile_scsi_get_rdev.c emile_second_get_configuration.c \
	  emile_second_set_configuration.c emile_second_get_next_property.c \
	  emile_second_get_property.c emile_second_set_property.c \
	  emile_second_remove_property.c emile_second_create_mapfile.c \
	  emile_second_set_param.c

HEADERS = emile.h libemile.h partition.h bootblock.h

CFLAGS  = -Wall -g
CPPFLAGS = -I../libmacos -I../libcontainer -I../libstream

all: $(LIBRARY)

include $(TOP)/../Rules.mk
