#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

all: first

first:	first.o
	$(OBJCOPY) -O binary first.o first

config.h:
	echo	".equ	base_address, $(BASE_ADDRESS)" > config.h.X
	mv config.h.X config.h

first.o:	first.S config.h
	$(AS) -o first.o first.S

clean:
	rm -f first *.o
