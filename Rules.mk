#
#  (c) 2005 Laurent Vivier <Laurent@lvivier.info>
#

OBJS	= $(patsubst %.sgml,%.8.gz,$(patsubst %.S,%.o,$(SOURCES:.c=.o)))

MODULE ?= $(shell basename $(TOP))

DISTFILES ?= $(SOURCES) $(HEADERS) Makefile

$(LIBRARY): $(LIBRARY)($(patsubst %.S,%.o,$(SOURCES:.c=.o)))

%.8.gz: %.8
	gzip -9c < $< > $@

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
	rm -f $(OBJS) $(PROGRAMS) $(LIBRARY) $(CLEAN) $(LIBRARIES) $(MANPAGES) $(MANPAGES:.8.gz=.8)
endif
