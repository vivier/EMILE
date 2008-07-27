#
# (c) 2005-2008 Laurent Vivier <Laurent@lvivier.info>
#
#

TOP=$(shell pwd)

SECTION5 = emile.conf.sgml

SECTION8 = emile-first-tune.sgml emile-install.sgml emile-map-set.sgml \
	   emile-set-cmdline.sgml emile-set-output.sgml emile.sgml \
	   emile-mkisofs.sgml

MANPATH ?= /usr/share/man/

all: man

include $(TOP)/../Rules.mk

man: $(MANPAGES)

install: all
	install -m 755 -d $(DESTDIR)/$(PREFIX)/$(MANPATH)/man5
	install -m 644 $(SECTION5:.sgml=.5.gz) \
	               $(DESTDIR)/$(PREFIX)/$(MANPATH)/man5
	install -m 755 -d $(DESTDIR)/$(PREFIX)/$(MANPATH)/man8
	install -m 644 $(SECTION8:.sgml=.8.gz) \
	               $(DESTDIR)/$(PREFIX)/$(MANPATH)/man8

uninstall:
	for file in $(SECTION5:.sgml=.5.gz) ; do \
	    rm -f $(DESTDIR)/$(PREFIX)/$(MANPATH)/man5/$${file}; \
	done
	for file in $(SECTION8:.sgml=.8.gz) ; do \
	    rm -f $(DESTDIR)/$(PREFIX)/$(MANPATH)/man8/$${file}; \
	done

include $(TOP)/../tools.mk
