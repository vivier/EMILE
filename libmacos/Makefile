#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP	= $(shell pwd)

CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -m68000 -Wa,-m68000 -Os
CPPFLAGS	= -I$(TOP) -DARCH_M68K

LIBRARY = libmacos.a

SOURCES = display_properties.S \
	  CloseDriver.c OpenDriver.c SerGetBuf.c

HEADERS = libmacos.h \
	  macos/devices.h macos/errors.h macos/events.h macos/files.h \
	  macos/gestalt.h macos/header macos/init.h macos/lowmem.h \
	  macos/memory.h macos/osutils.h macos/quickdraw.h macos/romdefs.h \
	  macos/scsi.h macos/serial.h macos/slotmanager.h macos/traps.h \
	  macos/types.h macos/video.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
