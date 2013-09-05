#
#  (c) 2005-2007 Laurent Vivier <Laurent@Vivier.EU>
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

LIBRARY = libconfig.a

SOURCES = config_get_indexed_property.c config_get_next_property.c \
	  config_get_property.c config_remove_property.c \
	  config_set_property.c config_set_indexed_property.c \
	  config_remove_indexed_property.c config_find_indexed_property.c \
	  config_find_entry.c config_add_property.c

HEADERS = libconfig.h

all:
	test -d $(TARGET) || mkdir $(TARGET)
	$(MAKE) -C $(TARGET) -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
