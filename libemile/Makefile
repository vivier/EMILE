LIBRARY = libemile.a

OBJS =	emile_first_get_param.o emile_first_set_param.o \
	emile_first_set_param_scsi.o emile_floppy_create_image.o \
	emile_scsi_create_container.o emile_second_get_cmdline.o \
	emile_second_get_kernel.o emile_second_get_output.o \
	emile_second_set_cmdline.o emile_second_set_kernel.o \
	emile_second_set_kernel_scsi.o emile_second_set_output.o

HEADERS = emile.h libemile.h emile-first.h

CFLAGS  = -Wall

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rc $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY)
