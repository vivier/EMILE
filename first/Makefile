#
#
# (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
#
#

all: first

first:	first.o
	$(OBJCOPY) -O binary first.o first

first.o:	first.S
	$(AS) -o first.o first.S

clean:
	rm -f first *.o
