TOP=$(shell pwd)

SOURCES	= emile-first-tune.sgml emile-install.sgml emile-map-set.sgml \
	  emile-set-cmdline.sgml emile-set-output.sgml emile.sgml

MANPAGES = $(SOURCES:.sgml=.8.gz)

MANPATH = /usr/share/man/man8

MANPAGESWPATH = $(foreach file, $(MANPAGES), $(MANPATH)/$(file))

all: $(MANPAGES)

install: all
	install -m 755 -d $(DESTDIR)/$(PREFIX)/$(MANPATH)
	install -m 644 *.8.gz $(DESTDIR)/$(PREFIX)/$(MANPATH)

uninstall:
	rm -f $(MANPAGESWPATH)

include $(TOP)/../Rules.mk
