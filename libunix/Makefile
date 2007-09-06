#
#  (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
#

TOP = $(shell pwd)

68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	+= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP) -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libunix.a

SOURCES = divsi3.S modsi3.S mulsi3.S udivsi3.S umodsi3.S free.c malloc.c \
	  memcpy.c memset.c printf.c putchar.c puts.c read.c sprintf.c \
	  strcpy.c strlen.c strncpy.c vsprintf.c write.c strcmp.c \
	  strncmp.c strtol.c strdup.c

HEADERS = 

all: $(LIBRARY)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
