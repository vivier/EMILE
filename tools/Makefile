#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PROGRAMS = emile-set-cmdline emile-first-info emile-first-tune emile-install \
	   emile-set-output emile

CPPFLAGS = -DSIGNATURE="\"$(SIGNATURE)\"" -I../libemile
CFLAGS	= -Wall
LDLIBS = -L../libemile -lemile

all: $(PROGRAMS)

clean:
	rm -f *.o $(PROGRAMS)
