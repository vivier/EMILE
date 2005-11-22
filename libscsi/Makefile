#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

TOP=$(shell pwd)
CFLAGS	= -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2
CPPFLAGS = -I$(TOP)/../libmacos -DARCH_M68K

LIBRARY = libscsi.a

SOURCES = scsi_read_sector.c scsi_command.c scsi_INQUIRY.c scsi_READ.c \
	  scsi_open.c scsi_close.c scsi_READ_CAPACITY.c

HEADERS = libscsi.h

all: $(LIBRARY)

include $(TOP)/../Rules.mk
