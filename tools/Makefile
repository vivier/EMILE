#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PROGRAMS = emile-set-cmdline emile-first-info emile-first-tune emile-install \
	   emile-set-output

CPPFLAGS = -DSIGNATURE="\"$(SIGNATURE)\""
CFLAGS	= -Wall

all: $(PROGRAMS)

emile-set-cmdline.o: emile-set-cmdline.c emile-first.h ../second/head.h emile.h

emile-set-output.o: emile-set-output.c emile-first.h ../second/head.h emile.h

emile-first-info.o: emile-first-info.c emile-first.h emile.h

emile-first-tune.o: emile-first-tune.c emile-first.h emile.h

emile-install.o: emile-install.c emile-first.h ../second/head.h emile.h

clean:
	rm -f *.o $(PROGRAMS)
