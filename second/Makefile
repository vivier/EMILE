# 
# 
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
# 
#

CPPFLAGS	= -DKERNEL_ARGS="\"$(KERNEL_ARGS)\"" -DVERSION="\"$(VERSION)\""
CFLAGS		= -Wno-multichar -O -m68030 -nostdlib -nodefaultlibs -Wall -Werror
ASFLAGS		= 
LS		= /bin/ls
AWK		= /bin/awk

RAMDISK		= ../ramdisk.gz
KERNEL_BIN	= ../vmlinux.bin
ZIPPED_KERNEL	= ../vmlinuz

KERNEL_SIZE=$(shell $(LS) -l $(KERNEL_BIN) | $(AWK) '{print $$5}')

ifeq ($(KERNEL_ARCH),PowerPC)
CPPFLAGS	+= -DTARGET_PPC
OBJS	= head.o MMU030_asm.o MMU040_asm.o image.o console.o printf.o \
	  font_8x16.o memory.o MMU030.o MMU040.o bootinfo.o misc.o glue.o \
	  bank.o arch.o
else
ifeq ($(KERNEL_ARCH),Motorola)
CPPFLAGS	+= -DTARGET_M68K
OBJS	= head.o MMU030_asm.o MMU040_asm.o image.o console.o printf.o \
	  font_8x16.o memory.o uncompress.o MMU030.o MMU040.o bootinfo.o \
	  misc.o glue.o \
	  enter_kernel030.o enter_kernel040.o bank.o arch.o
endif
endif

second: second.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got -j .got.plt \
			-O binary second.o second

second.o: $(OBJS) ld.script
	$(LD) -T ld.script -Ttext $(BASE_ADDRESS) \
		--defsym _KERNEL_SIZE=$(KERNEL_SIZE) -o second.o $(OBJS)

ifeq ($(shell ls $(RAMDISK) 2> /dev/null),$(RAMDISK))
image.o: main.o $(ZIPPED_KERNEL) $(RAMDISK)
	$(OBJCOPY) --add-section .image=$(ZIPPED_KERNEL) \
		   --add-section .ramdisk=$(RAMDISK) main.o image.o ;
else
image.o: main.o $(ZIPPED_KERNEL)
	$(OBJCOPY) --add-section .image=$(ZIPPED_KERNEL) main.o image.o;
endif

MMU030_asm.o: MMU030_asm.S
	$(AS) $(ASFLAGS) -m68030 -o $@ $^

enter_kernel030.o: enter_kernel030.S
	$(AS) $(ASFLAGS) -m68030 -o $@ $^

MMU040_asm.o: MMU040_asm.S
	$(AS) $(ASFLAGS) -m68040 -o $@ $^

enter_kernel040.o: enter_kernel040.S
	$(AS) $(ASFLAGS) -m68040 -o $@ $^

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^ 

clean:
	rm -f second *.o
