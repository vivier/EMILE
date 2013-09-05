#
#  (c) 2007 Laurent Vivier <Laurent@Vivier.EU>
#

TOP = $(shell pwd)

68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	+= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP) -DARCH_M68K

LIBRARY = libui.a

SOURCES = window.c progressbar.c scrolllist.c edit.c

HEADERS = libui.h

all: $(LIBRARY)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
