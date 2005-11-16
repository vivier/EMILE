#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

TOP	= $(shell pwd)
VPATH	= $(TOP)

SOURCES = first.S

HEADERS =

MEDIA = floppy

ifeq ($(MEDIA), scsi)
MEDIAFLAGS = --defsym SCSI_SUPPORT=1
TARGET = scsi
endif

ifeq ($(MEDIA), floppy)
MEDIAFLAGS = --defsym FLOPPY_SUPPORT=1
TARGET = floppy
endif

ifeq ($(MEDIA), ata)
MEDIAFLAGS = --defsym ATA_SUPPORT=1
TARGET = ata
endif

PROGRAMS = first

all: $(TARGET)/first

first:	first.o
	$(OBJCOPY) -O binary $^ $@

first.o: first.S
	$(AS) $(ASFLAGS) $(MEDIAFLAGS) -o $@ $^

$(TARGET)/first::
	test -d $(TARGET) || mkdir $(TARGET)
	cd $(TARGET) && make -f $(TOP)/Makefile TOP=$(TOP) AS=$(AS) \
				OBJCOPY=$(OBJCOPY) "MEDIAFLAGS=$(MEDIAFLAGS)" \
				first

install::
	install -d $(DESTDIR)/$(PREFIX)/boot/emile/
	install scsi/first $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install floppy/first $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy

uninstall::
	rm -f $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy

include $(TOP)/../Rules.mk
