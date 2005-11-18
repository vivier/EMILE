#
#  (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
#

OBJS	= $(patsubst %.S,%.o,$(SOURCES:.c=.o))

MODULE ?= $(basename $(TOP))

DISTFILES ?= $(SOURCES) $(HEADERS) $(MANPAGES) Makefile

%.8: %.sgml
	docbook-to-man $< > $@

%.8.gz: %.8
	gzip -9c $< > $@

ifdef LIBRARY
$(LIBRARY): $(OBJS)
	$(AR) rc $@ $^
endif

dist:
	for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/$(MODULE)/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/$(MODULE)/$$file; \
	done

ifdef TARGET
clean:
	cd $(TARGET) && \
	rm -f $(OBJS) $(PROGRAMS) $(LIBRARY) $(MANPAGES)
else
clean:
	rm -f $(OBJS) $(PROGRAMS) $(LIBRARY) $(MANPAGES)
endif
