#
#
# (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
#
#

first_scsi:	ASFLAGS += --defsym SCSI_SUPPORT=1
first_floppy:	ASFLAGS += --defsym FLOPPY_SUPPORT=1

TOP	= $(shell pwd)
VPATH	= $(TOP)

ASFLAGS = -I$(TOP)

SOURCES = first.S
CLEAN	= first_floppy.o first_scsi.o

HEADERS = floppy.i macos.i scsi.i 32bitmode.i

PROGRAMS = first_scsi first_floppy

.PHONY: all

all: $(PROGRAMS)

%: %.o
	$(OBJCOPY) -O binary $^ $@

first_scsi.o first_floppy.o first_ata.o: first.S
	$(AS) $(ASFLAGS) -o $@ $^

install::
	install -d $(DESTDIR)/$(PREFIX)/boot/emile/
	install first_scsi $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install first_floppy $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy

uninstall::
	rm -f $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy

include $(TOP)/../Rules.mk
