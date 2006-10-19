#
#
# (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
#
#

TOP	= $(shell pwd)
VPATH	= $(TOP)

ASFLAGS = -I$(TOP)

CFLAGS          = $(OPT_CFLAGS) -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2

SOURCES = head.S 

.PHONY: all

all: apple_driver

apple_driver: apple_driver.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got \
			-O binary apple_driver.o apple_driver

apple_driver.o:	head.o
	$(LD) -o apple_driver.o head.o

.S.o:
	 $(AS) $(ASFLAGS) -o $@ $^

install::
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install first_floppy $(DESTDIR)/$(PREFIX)/lib/emile/apple_driver

uninstall::
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/apple_driver

include $(TOP)/../Rules.mk
