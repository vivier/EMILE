#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

TOP	= $(shell pwd)
VPATH	= $(TOP)

all: first_floppy first_scsi

first:	first.o
	$(OBJCOPY) -O binary $^ $@

first.o:	first.S
	$(AS) $(ASFLAGS) $(SCSIFLAGS) -o $@ $^

first_floppy::
	test -d floppy || mkdir floppy
	cd floppy && make -f $(TOP)/Makefile TOP=$(TOP) AS=$(AS) \
				OBJCOPY=$(OBJCOPY) first
	mv floppy/first first_floppy

first_scsi::
	test -d scsi || mkdir scsi
	cd scsi && make -f $(TOP)/Makefile TOP=$(TOP) AS=$(AS) \
			OBJCOPY=$(OBJCOPY) SCSIFLAGS="--defsym SCSI_SUPPORT=1" first
	mv scsi/first first_scsi

clean:
	rm -f first_floppy first_scsi floppy/*.o scsi/*.o
