#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PROGRAMS = emile-set-cmdline emile-first-info emile-first-tune emile-install \
	   emile-set-output emile

CPPFLAGS = -DSIGNATURE="\"$(SIGNATURE)\"" -DPREFIX=\"$(PREFIX)\" -I../libemile
CFLAGS	= -Wall -g
LDLIBS = -L../libemile -lemile

all: $(PROGRAMS)

emile: emile.o emile_scanbus.o

clean:
	rm -f *.o $(PROGRAMS)
