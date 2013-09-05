#
# (c) 2005-2008 Laurent Vivier <Laurent@Vivier.EU>
#
#

TOP=$(shell pwd)

MODULE ?= $(shell basename $(TOP))

SECTION5 = emile.conf.sgml

SECTION8 = emile-first-tune.sgml emile-install.sgml emile-map-set.sgml \
	   emile-set-cmdline.sgml emile-set-output.sgml emile.sgml \
	   emile-mkisofs.sgml

MANPATH ?= /usr/share/man/

MANPAGES = $(SECTION5:.sgml=.5.gz) $(SECTION8:.sgml=.8.gz)

DISTFILES ?= $(SECTION5) $(SECTION8) Makefile

all: man

%.gz: %
	gzip -9f $<

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

clean:
	rm -f $(MANPAGES) $(MANPAGES:.8.gz=.8) $(MANPAGES:.5.gz=.5)

dist:
	@echo TAR $(MODULE)
	@for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/$(MODULE)/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/$(MODULE)/$$file; \
	done

include $(TOP)/../tools.mk
