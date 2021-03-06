#
#
#  (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
#
#

TARGET = m68k-linux
MEDIA = floppy

TOP		= $(shell pwd)
VPATH		= $(TOP)
CPPFLAGS	= -DVERSION="\"$(VERSION)\"" -I$(TOP) -Wa,-I$(TOP) \
		  $(OPT_CPPFLAGS) -DUSE_CLI \
		  -I$(TOP)/../libmacos -Wa,-I$(TOP)/../libmacos \
		  -I$(TOP)/../libstream -I$(TOP)/../libui -I$(TOP)/../libconfig

# -O2 is needed to be able to inline functions from libmacos
CFLAGS		= $(OPT_CFLAGS) -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic -O2
ASFLAGS		=
LIBS		= $(OPT_LIBS) \
		  -L$(TOP)/../libiso9660/m68k-linux -liso9660 \
		  -L$(TOP)/../libext2/m68k-linux -lext2 \
		  -L$(TOP)/../libunix -lunix \
		  -L$(TOP)/../libmacos -lmacos -lunix \
		  -L$(TOP)/../libgzip/m68k-linux -lgzip \
		  -L$(TOP)/../libfloppy -lfloppy -L$(TOP)/../libscsi -lscsi \
		  -L$(TOP)/../libblock -lblock \
		  -L$(TOP)/../libcontainer -lcontainer -lunix \
		  -L$(TOP)/../libui -lui \
		  -L$(TOP)/../libconfig/m68k-linux -lconfig -lunix \
		  -L$(TOP)/../libmap/m68k-linux -lmap

LS		= ls
AWK		= awk

HEADERS	= arch.h bank.h bootenv.h bootinfo.h bootx.h console.h driver.h enter_kernel030.h enter_kernel040.h enter_kernel.h enter_kernelnoMMU.h switch_to_PPC.h head.h keyboard.h load.h misc.h MMU030.h MMU040.h serial.h vga.h config.h copymem.i cli.h

SOURCES = head.S main.c console.c \
	  font_8x16.c \
	  misc.c bank.c arch.c \
	  load.c serial.c vga.c driver.c \
	  enter_kernel.c config.c cli.c

SOURCES_CLI = keyboard.c

OBJS	= $(patsubst %.c,%.o,$(SOURCES:.S=.o)) $(SOURCES_CLI:.c=.o) \
	  $(OPTOBJS)

# ARCHITECTURE

SOURCES_MMU030 = asm_MMU030.S MMU030.c enter_kernel030.S
SOURCES_MMU040 = asm_MMU040.S MMU040.c enter_kernel040.S
SOURCES_MMU = $(SOURCES_MMU030) $(SOURCES_MMU040)
SOURCES_noMMU = enter_kernelnoMMU.S
SOURCES_M68K = $(SOURCES_MMU) $(SOURCES_noMMU)

OBJS_M68K = $(patsubst %.c,%.o,$(SOURCES_M68K:.S=.o))

SOURCES_PPC = switch_to_PPC.S

OBJS_PPC = $(patsubst %.c,%.o,$(SOURCES_PPC:.S=.o))

# TARGET

SOURCES_LINUX_M68K = bootinfo.c
OBJS_LINUX_M68K = $(SOURCES_LINUX_M68K:.c=.o)

SOURCES_NETBSD_M68K = bootenv.c
OBJS_NETBSD_M68K = $(SOURCES_NETBSD_M68K:.c=.o)

SOURCES_LINUX_PPC = bootx.c
OBJS_LINUX_PPC = $(SOURCES_LINUX_PPC:.c=.o)

DISTFILES = $(SOURCES) $(SOURCES_M68K) $(SOURCES_PPC) $(SOURCES_CLI) $(SOURCES_LINUX_M68K) $(SOURCES_NETBSD_M68K) $(SOURCES_LINUX_PPC) $(HEADERS) Makefile ld.script

ifeq ("$(TARGET)", "m68k-linux")

	OBJS_TARGET	= $(OBJS_M68K) $(OBJS_LINUX_M68K)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_M68K -DUSE_MMU030 -DUSE_MMU040
	CFLAGS_TARGET	= -Os

else
ifeq ("$(TARGET)", "classic-linux")

	OBJS_TARGET 	= $(OBJS_noMMU) $(OBJS_LINUX_M68K)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_M68K
	CFLAGS_TARGET	= -m68000 -Wa,-m68000 -Os

else
ifeq ("$(TARGET)", "ppc-linux")

	OBJS_TARGET 	= $(OBJS_PPC) $(OBJS_LINUX_PPC)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_PPC
	CFLAGS_TARGET	= -Os

else
ifeq ("$(TARGET)", "mixed-linux")

	OBJS_TARGET 	= $(OBJS_M68K) $(OBJS_LINUX_M68K) $(OBJS_PPC) $(OBJS_LINUX_PPC)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_M68K -DUSE_MMU030 -DUSE_MMU040 -DARCH_PPC
	CFLAGS_TARGET	= -Os

else
ifeq ("$(TARGET)", "m68k-netbsd")

	OBJS_TARGET 	= $(OBJS_M68K) $(OBJS_NETBSD_M68K)
	CPPFLAGS_TARGET = -D__NETBSD__ -DARCH_M68K -DUSE_MMU030 -DUSE_MMU040
	CFLAGS_TARGET	= -Os

endif
endif
endif
endif
endif

LIBS_MEDIA=-L$(TOP)/../libstream/$(MEDIA) -lstream

$(TARGET)-$(MEDIA)/second::
	test -d $(TARGET)-$(MEDIA) || mkdir $(TARGET)-$(MEDIA)
	cd $(TARGET)-$(MEDIA) && make -f $(TOP)/Makefile second \
			VERSION=$(VERSION) \
			TOP=$(TOP) \
			OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) \
			AS=$(AS) PPC_OBJCOPY=$(PPC_OBJCOPY) \
			PPC_CC=$(PPC_CC) \
			SIGNATURE="$(SIGNATURE)" \
			OPT_CPPFLAGS="$(CPPFLAGS_TARGET) $(CPPFLAGS_MEDIA)" \
			OPT_CFLAGS="$(CFLAGS_TARGET)" \
			OPTOBJS="$(OBJS_TARGET) $(OBJS_MEDIA)" \
			OPT_LIBS="$(LIBS_MEDIA)"

$(TARGET)-$(MEDIA)-driver/apple_driver::
	test -d $(TARGET)-$(MEDIA)-driver || mkdir $(TARGET)-$(MEDIA)-driver
	cd $(TARGET)-$(MEDIA)-driver && make -f $(TOP)/Makefile apple_driver \
			VERSION=$(VERSION) \
			TOP=$(TOP) \
			OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) \
			AS=$(AS) PPC_OBJCOPY=$(PPC_OBJCOPY) \
			PPC_CC=$(PPC_CC) \
			SIGNATURE="$(SIGNATURE)" \
			OPT_CPPFLAGS="-DAPPLE_DRIVER $(CPPFLAGS_TARGET) $(CPPFLAGS_MEDIA)" \
			OPT_CFLAGS="$(CFLAGS_TARGET)" \
			OPTOBJS="$(OBJS_TARGET) $(OBJS_MEDIA)" \
			OPT_LIBS="$(LIBS_MEDIA)"

ifeq ("$(TARGET)", "ppc-linux")
bootstrapPPC.o:	bootstrapPPC.S
	$(PPC_CC) $(CPPFLAGS) -c $(TOP)/bootstrapPPC.S -o bootstrapPPC.o

bootstrap: bootstrapPPC.o
	$(PPC_OBJCOPY) -j .text -j .data -j .rodata \
			-O binary bootstrapPPC.o bootstrap

switch_to_PPC.o: switch_to_PPC.S bootstrap
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^
	$(OBJCOPY) --add-section .bootstrap=bootstrap \
		   --set-section-flags .bootstrap=contents,alloc,load,data \
		   switch_to_PPC.o

apple_driver: apple_driver.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got  -j .bootstrap \
			-O binary apple_driver.o apple_driver
second: second.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got  -j .bootstrap \
			-O binary second.o second
else
apple_driver: apple_driver.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got \
			-O binary apple_driver.o apple_driver
second: second.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got \
			-O binary second.o second
endif

second.o: $(OBJS) $(TOP)/ld.script
	$(LD) -T $(TOP)/ld.script -o second.o $(OBJS) $(LIBS)

apple_driver.o: $(OBJS) $(TOP)/ld.script
	$(LD) -T $(TOP)/ld.script -o apple_driver.o $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

%030.o: %030.S; $(CC) $(CPPFLAGS) $(CFLAGS) -m68030 -Wa,-mcpu=68030 -c -o $@ $^
%040.o: %040.S; $(CC) $(CPPFLAGS) $(CFLAGS) -m68040 -Wa,-mcpu=68040 -c -o $@ $^

install::
	install -d $(DESTDIR)/$(PREFIX)/boot/emile/
	install $(KARCH)-linux-scsi/second $(DESTDIR)/$(PREFIX)/boot/emile/$(KARCH)-second_scsi
	install -d $(DESTDIR)/$(PREFIX)/lib/emile/
	install $(KARCH)-linux-floppy/second $(DESTDIR)/$(PREFIX)/lib/emile/$(KARCH)-second_floppy
	install m68k-linux-scsi-driver/apple_driver $(DESTDIR)/$(PREFIX)/lib/emile/apple_driver

uninstall::
	rm -f $(DESTDIR)/$(PREFIX)/boot/emile/$(KARCH)-second_scsi
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/$(KARCH)-second_floppy
	rm -f $(DESTDIR)/$(PREFIX)/lib/emile/apple_driver

dist:
	@echo TAR second
	@for file in $(DISTFILES); do \
		dir=$$(dirname $$file); \
		if [ "$$dir" != "" ] ; then \
			mkdir -p $(DISTDIR)/second/$$dir; \
		fi; \
		cp -p $$file $(DISTDIR)/second/$$file; \
	done

clean:
	rm -fr m68k-linux-floppy m68k-linux-scsi m68k-linux-ata \
	      ppc-linux-floppy ppc-linux-scsi ppc-linux-ata \
	      mixed-linux-floppy mixed-linux-scsi mixed-linux-ata \
	      m68k-netbsd-floppy classic-linux-floppy m68k-linux-full \
	      ppc-linux-full mixed-linux-full m68k-netbsd-full \
	      m68k-linux-scsi-driver

include $(TOP)/../tools.mk
