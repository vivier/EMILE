#
#  (c) 2005 Laurent Vivier <Laurent@lvivier.info>
#

TOP = $(shell pwd)

68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	+= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP) -DARCH_M68K

LIBRARY = libui.a

SOURCES = window.c progressbar.c scrolllist.c

HEADERS = libui.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
