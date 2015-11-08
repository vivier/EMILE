#
#  (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
#
#

TOP=$(shell pwd)
VPATH=$(TOP)

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS)
LDFLAGS = $(CROSS_COMPILE_LDFLAGS)

LIBRARY = libemile.a

SOURCES = emile_first_get_param.c \
	  emile_first_set_param.c emile_first_set_param_scsi.c \
	  emile_floppy_create_image.c emile_second_create_mapfile.c \
	  emile_scsi_create_container.c emile_scsi_get_dev.c \
	  emile_scsi_get_rdev.c emile_second_get_configuration.c \
	  emile_second_set_configuration.c \
	  emile_second_set_param.c emile_second_get_param.c \
	  emile_first_set_param_scsi_extents.c

HEADERS = emile.h libemile.h bootblock.h

CFLAGS  = -Wall -Werror -g
CPPFLAGS = -I$(TOP)/../libmacos -I$(TOP)/../libcontainer -I$(TOP)/../libstream -I$(TOP)/../libconfig -I$(TOP)/../libmap -I$(TOP)

all:
	test -d $(TARGET) || mkdir $(TARGET)
	$(MAKE) -C $(TARGET) -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
