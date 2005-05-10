#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar

PROGRAMS = emile-set-cmdline emile-first-tune emile-install \
	   emile-set-output emile emile-map-set

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS) -DSIGNATURE="\"$(SIGNATURE)\"" -DPREFIX=\"$(PREFIX)\" -I../libemile
CFLAGS	= -Wall -g
LDLIBS = $(CROSS_COMPILE_LDFLAGS) -L../libemile -lemile

all: $(PROGRAMS)

emile: emile.o emile_scanbus.o

emile-map-set: emile-map-set.o emile_scanbus.o

clean:
	rm -f *.o $(PROGRAMS)
