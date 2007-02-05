#
#
# (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
#
#

TOP	= $(shell pwd)
VPATH	= $(TOP)

CPPFLAGS = -DARCH_M68K -I$(TOP)/../libmacos -Wa,-I$(TOP)/../libmacos

ASFLAGS = -I$(TOP)

CFLAGS	+= $(OPT_CFLAGS) -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2
LIBS	= -L$(TOP)/../libunix -lunix -L$(TOP)/../libmacos -lmacos

SOURCES	= head.S main.c console.c vga.c font_8x16.c

HEADERS = vga.h console.h

PROGRAMS	= apple_driver

CLEAN		= apple_driver.o 

.PHONY: all

all: apple_driver

include $(TOP)/../Rules.mk

apple_driver: apple_driver.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got \
			-O binary apple_driver.o apple_driver

apple_driver.o:	$(OBJS) ld.script
	$(LD) -T ld.script -o apple_driver.o $(OBJS) $(LIBS)

.S.o:
	$(AS) $(ASFLAGS) -o $@ $^

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

install::
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install apple_driver $(DESTDIR)/$(PREFIX)/lib/emile/apple_driver

uninstall::
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/apple_driver

