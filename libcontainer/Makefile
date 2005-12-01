#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP=$(shell pwd)
VPATH=$(TOP)

TARGET = native

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -m68000 -Wa,-m68000 -Os
CPPFLAGS = 

LIBRARY = libcontainer.a

SOURCES = container_close.c container_lseek.c container_open.c \
	  container_read.c

HEADERS = libcontainer.h

all:
	test -d $(TARGET) || mkdir $(TARGET)
	cd $(TARGET) && make -f $(TOP)/Makefile $(LIBRARY) TOP=$(TOP)


include $(TOP)/../Rules.mk
