TOP=$(shell pwd)

MANPAGES = emile.8.gz emile-install.8.gz emile-set-cmdline.8.gz \
	   emile-first-tune.8.gz emile-map-set.8.gz emile-set-output.8.gz 

MANPATH = /usr/share/man/man8

MANPAGESWPATH = $(foreach file, $(MANPAGES), $(MANPATH)/$(file))

all: $(MANPAGES)

install: all
	install -m 755 -d $(DESTDIR)/$(PREFIX)/$(MANPATH)
	install -m 644 *.8.gz $(DESTDIR)/$(PREFIX)/$(MANPATH)

uninstall:
	rm -f $(MANPAGESWPATH)

include $(TOP)/../Rules.mk
