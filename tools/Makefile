#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

PROGRAMS = emile-set-cmdline emile-first-info emile-first-tune

CFLAGS	= -Wall

all: $(PROGRAMS)

emile-set-cmdline.o: emile-set-cmdline.c emile-first.h

emile-first-info.o: emile-first-info.c emile-first.h

emile-first-tune.o: emile-first-tune.c emile-first.h

clean:
	rm -f *.o $(PROGRAMS)
