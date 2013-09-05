#
#  (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
#
#

TOP=$(shell pwd)

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS)
LDFLAGS = $(CROSS_COMPILE_LDFLAGS)

LIBRARY = libemile.a

SOURCES = emile_checksum.c emile_first_get_param.c \
	  emile_first_set_param.c emile_first_set_param_scsi.c \
	  emile_floppy_create_image.c emile_second_create_mapfile.c \
	  emile_scsi_create_container.c emile_scsi_get_dev.c \
	  emile_scsi_get_rdev.c emile_second_get_configuration.c \
	  emile_second_set_configuration.c \
	  emile_second_set_param.c emile_second_get_param.c \
	  emile_first_set_param_scsi_extents.c

HEADERS = emile.h libemile.h bootblock.h

CFLAGS  = -Wall -Werror -g
CPPFLAGS = -I../libmacos -I../libcontainer -I../libstream -I../libconfig -I../libmap -I.

all: $(LIBRARY)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
