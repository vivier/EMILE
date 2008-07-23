#
#  (c) 2005 Laurent Vivier <Laurent@lvivier.info>
#

OBJS	 = $(patsubst %.S,%.o,$(SOURCES:.c=.o))
MANPAGES = $(SECTION5:.sgml=.5.gz) $(SECTION8:.sgml=.8.gz)

MODULE ?= $(shell basename $(TOP))

DISTFILES ?= $(SOURCES) $(SECTION5) $(SECTION8) $(HEADERS) Makefile

$(LIBRARY): $(LIBRARY)($(patsubst %.S,%.o,$(SOURCES:.c=.o)))

%.gz: %
	gzip -9f $<

dist:
	@echo TAR $(MODULE)
	@for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/$(MODULE)/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/$(MODULE)/$$file; \
	done

ifdef TARGET
clean:
	(cd $(TARGET) && rm -f $(OBJS) $(PROGRAMS) $(LIBRARY)) || true
else
clean:
	rm -f $(OBJS) $(PROGRAMS) $(LIBRARY) $(CLEAN) $(LIBRARIES) \
	      $(MANPAGES) $(MANPAGES:.8.gz=.8) $(MANPAGES:.5.gz=.5)
endif
