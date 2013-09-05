#
#  (c) 2005-2007 Laurent Vivier <Laurent@Vivier.EU>
#

TOP=$(shell pwd)
68000FLAGS = -m68000 -Wa,-m68000
CFLAGS	+= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2 -Os
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libscsi.a

SOURCES = scsi_read_sector.c scsi_command.c scsi_INQUIRY.c scsi_READ.c \
	  scsi_open.c scsi_close.c scsi_READ_CAPACITY.c scsi_get_blocksize.c \
	  scsi_TEST_UNIT_READY.c scsi_INQUIRY.c scsi_REQUEST_SENSE.c

HEADERS = libscsi.h

all: $(LIBRARY)

include $(TOP)/../tools.mk
include $(TOP)/../Rules.mk
