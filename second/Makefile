# 
# 
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
# 
#

TOP		= $(shell pwd)
VPATH		= $(TOP)
CPPFLAGS	= -DVERSION="\"$(VERSION)\"" -I$(TOP) -Wa,-I$(TOP) \
		  $(OPTFLAGS)
CFLAGS		= -Wno-multichar -O -m68030 -nostdlib -nodefaultlibs -Wall -Werror -fpic
ASFLAGS		= 
LS		= ls
AWK		= awk

OBJS	= head.o MMU030_asm.o MMU040_asm.o main.o console.o printf.o \
	  font_8x16.o memory.o uncompress.o MMU030.o MMU040.o bootinfo.o \
	  misc.o glue.o enter_kernel030.o enter_kernel040.o bank.o arch.o \
	  load.o serial.o vga.o $(OPTOBJS)

OBJS_SCSI = scsi.o container.o

all: m68k-second_floppy m68k-second_scsi ppc-second_floppy ppc-second_scsi

second: second.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got \
			-O binary second.o second

m68k-second_floppy::
	test -d m68k-floppy || mkdir m68k-floppy
	cd m68k-floppy && make -f $(TOP)/Makefile second VERSION=$(VERSION) \
			     TOP=$(TOP) OPTFLAGS="-DARCH_M68K" \
			     OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) \
			     AS=$(AS) SIGNATURE="$(SIGNATURE)"
	mv m68k-floppy/second m68k-second_floppy

m68k-second_scsi::
	test -d m68k-scsi || mkdir m68k-scsi
	cd m68k-scsi && make -f $(TOP)/Makefile second VERSION=$(VERSION) \
			   TOP=$(TOP) OPTFLAGS="-DARCH_M68K -DSCSI_SUPPORT" \
			   OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) \
			   SIGNATURE="$(SIGNATURE)" OPTOBJS="$(OBJS_SCSI)"
	mv m68k-scsi/second m68k-second_scsi

ppc-second_floppy::
	test -d ppc-floppy || mkdir ppc-floppy
	cd ppc-floppy && make -f $(TOP)/Makefile second VERSION=$(VERSION) \
			     TOP=$(TOP) OPTFLAGS="-DARCH_PPC" \
			     OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) \
			     AS=$(AS) SIGNATURE="$(SIGNATURE)"
	mv ppc-floppy/second ppc-second_floppy

ppc-second_scsi::
	test -d ppc-scsi || mkdir ppc-scsi
	cd ppc-scsi && make -f $(TOP)/Makefile second VERSION=$(VERSION) \
			   TOP=$(TOP) OPTFLAGS="-DARCH_PPC -DSCSI_SUPPORT" \
			   OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) AS=$(AS) \
			   SIGNATURE="$(SIGNATURE)" OPTOBJS="$(OBJS_SCSI)"
	mv ppc-scsi/second ppc-second_scsi

second.o: $(OBJS) $(TOP)/ld.script
	$(LD) -T $(TOP)/ld.script -o second.o $(OBJS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^ 

clean:
	rm -f second_floppy second_scsi floppy/*.o scsi/*.o
