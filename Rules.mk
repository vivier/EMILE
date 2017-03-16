#
#  (c) 2005-2013 Laurent Vivier <Laurent@Vivier.EU>
#

OBJS	 = $(patsubst %.S,%.o,$(SOURCES:.c=.o))

MODULE ?= $(shell basename $(TOP))

DISTFILES ?= $(SOURCES) $(HEADERS) Makefile

ARFLAGS+=-U
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
	rm -f $(OBJS) $(PROGRAMS) $(LIBRARY) $(CLEAN) $(LIBRARIES)
endif

ifeq ($(shell readlink -e /usr/include/linux/ext2_fs.h),)
CPPFLAGS += -DCONFIG_E2FSLIBS
endif
