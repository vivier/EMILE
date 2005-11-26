#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP = $(shell pwd)

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -m68000 -Wa,-m68000 -Os
CPPFLAGS = -I$(TOP) -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libunix.a

SOURCES = divsi3.S modsi3.S mulsi3.S udivsi3.S umodsi3.S free.c malloc.c \
	  memcpy.c memset.c printf.c putchar.c puts.c read.c sprintf.c \
	  strcpy.c strlen.c strncpy.c vsprintf.c write.c strcmp.c \
	  strncmp.c strtol.c

HEADERS = 

all: $(LIBRARY)

include $(TOP)/../Rules.mk
