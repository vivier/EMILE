# 
# 
#  (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
# 
#

CPPFLAGS	= -DVERSION="\"$(VERSION)\""
CFLAGS		= -Wno-multichar -O -m68030 -nostdlib -nodefaultlibs -Wall -Werror -fpic
ASFLAGS		= 
LS		= ls
AWK		= awk

ifeq ($(KERNEL_ARCH),PowerPC)
CPPFLAGS	+= -DTARGET_PPC
OBJS	= head.o MMU030_asm.o MMU040_asm.o main.o console.o printf.o \
	  font_8x16.o memory.o MMU030.o MMU040.o bootinfo.o misc.o glue.o \
	  bank.o arch.o load.o
else
ifeq ($(KERNEL_ARCH),Motorola)
CPPFLAGS	+= -DTARGET_M68K
OBJS	= head.o MMU030_asm.o MMU040_asm.o main.o console.o printf.o \
	  font_8x16.o memory.o uncompress.o MMU030.o MMU040.o bootinfo.o \
	  misc.o glue.o enter_kernel030.o enter_kernel040.o bank.o arch.o \
	  load.o
endif
endif

second: second.o
	$(OBJCOPY) -j .text -j .data -j .rodata -j .got \
			-O binary second.o second

second.o: $(OBJS) ld.script
	$(LD) -T ld.script -o second.o $(OBJS)

MMU030_asm.o: MMU030_asm.S
	$(AS) $(ASFLAGS) -m68030 -o $@ $^

enter_kernel030.o: enter_kernel030.S copymem.i
	$(AS) $(ASFLAGS) -m68030 -o $@ $<

MMU040_asm.o: MMU040_asm.S
	$(AS) $(ASFLAGS) -m68040 -o $@ $^

enter_kernel040.o: enter_kernel040.S copymem.i
	$(AS) $(ASFLAGS) -m68040 -o $@ $<

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^ 

clean:
	rm -f second *.o
