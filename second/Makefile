# 
# 
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
# 
#

TARGET = m68k-linux
MEDIA = floppy

TOP		= $(shell pwd)
VPATH		= $(TOP)
CPPFLAGS	= -DVERSION="\"$(VERSION)\"" -I$(TOP) -Wa,-I$(TOP) $(OPT_CPPFLAGS) -DUSE_CLI
CFLAGS		= $(OPT_CFLAGS) -nostdlib -nodefaultlibs -Wall -Werror -Wno-multichar -fpic
ASFLAGS		= 
LS		= ls
AWK		= awk

OBJS	= head.o main.o console.o printf.o \
	  font_8x16.o memory.o uncompress.o \
	  misc.o glue.o bank.o arch.o \
	  load.o serial.o vga.o driver.o \
	  enter_kernel.o \
	  $(OPTOBJS) $(OBJS_CLI)

OBJS_CLI = keyboard.o cli.o

# ARCHITECTURE

OBJS_MMU030 = MMU030_asm.o MMU030.o enter_kernel030.o
OBJS_MMU040 = MMU040_asm.o MMU040.o enter_kernel040.o
OBJS_MMU = $(OBJS_MMU030) $(OBJS_MMU040)
OBJS_noMMU = enter_kernelnoMMU.o
OBJS_M68K = $(OBJS_MMU) $(OBJS_noMMU)

OBJS_PPC = enter_kernelPPC.o PPC_asm.o

# TARGET

OBJS_LINUX_M68K = bootinfo.o

OBJS_NETBSD_M68K = bootenv.o

OBJS_LINUX_PPC = bootx.o

# MEDIA

OBJS_SCSI = scsi.o container.o

OBJS_FLOPPY = 

OBJS_ATA = 


ifeq ("$(TARGET)", "m68k-linux")

	OBJS_TARGET	= $(OBJS_M68K) $(OBJS_LINUX_M68K)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_M68K -DUSE_MMU030 -DUSE_MMU040
	CFLAGS_TARGET	= -m68020 -Wa,-m68020 -Os

else
ifeq ("$(TARGET)", "classic-linux")

	OBJS_TARGET 	= $(OBJS_noMMU) $(OBJS_LINUX_M68K)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_M68K
	CFLAGS_TARGET	= -m68000 -Wa,-m68000 -Os

else
ifeq ("$(TARGET)", "ppc-linux")

	OBJS_TARGET 	= $(OBJS_PPC) $(OBJS_LINUX_PPC)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_PPC
	CFLAGS_TARGET	= -m68020 -Wa,-m68020 -Os

else
ifeq ("$(TARGET)", "mixed-linux")

	OBJS_TARGET 	= $(OBJS_M68K) $(OBJS_LINUX_M68K) $(OBJS_PPC) $(OBJS_LINUX_PPC)
	CPPFLAGS_TARGET = -D__LINUX__ -DARCH_M68K -DUSE_MMU030 -DUSE_MMU040 -DARCH_PPC
	CFLAGS_TARGET	= -m68020 -Wa,-m68020 -Os

else
ifeq ("$(TARGET)", "m68k-netbsd")

	OBJS_TARGET 	= $(OBJS_M68K) $(OBJS_NETBSD_M68K)
	CPPFLAGS_TARGET = -D__NETBSD__ -DARCH_M68K -DUSE_MMU030 -DUSE_MMU040
	CFLAGS_TARGET	= -m68020 -Wa,-m68020 -Os

endif
endif
endif
endif
endif

ifeq ("$(MEDIA)", "scsi")

	OBJS_MEDIA = $(OBJS_SCSI)
	CPPFLAGS_MEDIA = -DSCSI_SUPPORT

else
ifeq ("$(MEDIA)", "floppy")

	OBJS_MEDIA = $(OBJS_FLOPPY)
	CPPFLAGS_MEDIA = -DFLOPPY_SUPPORT

else
ifeq ("$(MEDIA)", "ata")

	OBJS_MEDIA = $(OBJS_ATA)
	CPPFLAGS_MEDIA = -DATA_SUPPORT

endif
endif
endif

$(TARGET)-$(MEDIA)/second::
	test -d $(TARGET)-$(MEDIA) || mkdir $(TARGET)-$(MEDIA)
	cd $(TARGET)-$(MEDIA) && make -f $(TOP)/Makefile second \
			VERSION=$(VERSION) \
			TOP=$(TOP) \
			OBJCOPY=$(OBJCOPY) LD=$(LD) CC=$(CC) \
			AS=$(AS) SIGNATURE="$(SIGNATURE)" \
			OPT_CPPFLAGS="$(CPPFLAGS_TARGET) $(CPPFLAGS_MEDIA)" \
			OPT_CFLAGS="$(CFLAGS_TARGET)" \
			OPTOBJS="$(OBJS_TARGET) $(OBJS_MEDIA)"

second: second.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got \
			-O binary second.o second

second.o: $(OBJS) $(TOP)/ld.script
	$(LD) -T $(TOP)/ld.script -o second.o $(OBJS)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^ 

clean:
	rm -fr m68k-linux-floppy m68k-linux-scsi m68k-linux-ata \
	      ppc-linux-floppy ppc-linux-scsi ppc-linux-ata \
	      mixed-linux-floppy mixed-linux-scsi mixed-linux-ata \
	      m68k-netbsd-floppy
