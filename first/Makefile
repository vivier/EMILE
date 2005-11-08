#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

TOP	= $(shell pwd)
VPATH	= $(TOP)

DISTFILES = Makefile first.S

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
			OBJCOPY=$(OBJCOPY) SCSIFLAGS="--defsym SCSI_SUPPORT=1" \
			first
	mv scsi/first first_scsi

install::
	install -d $(DESTDIR)/$(PREFIX)/boot/emile/
	install first_scsi $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install first_floppy $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy

uninstall::
	rm -f $(DESTDIR)/$(PREFIX)/boot/emile/first_scsi
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/first_floppy

dist:
	for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/first/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/first/$$file; \
	done

clean:
	rm -f first_floppy first_scsi floppy/*.o scsi/*.o
