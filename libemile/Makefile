AS=$(CROSS_COMPILE)as
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar

CPPFLAGS = $(CROSS_COMPILE_CPPFLAGS)
LDFLAGS = $(CROSS_COMPILE_LDFLAGS)

LIBRARY = libemile.a

OBJS =	emile_first_get_param.o emile_first_set_param.o \
	emile_first_set_param_scsi.o emile_floppy_create_image.o \
	emile_scsi_create_container.o emile_second_get_cmdline.o \
	emile_second_get_kernel.o emile_second_get_output.o \
	emile_second_set_cmdline.o emile_second_set_kernel.o \
	emile_second_set_kernel_scsi.o emile_second_set_output.o \
	emile_second_get_buffer_size.o emile_second_set_buffer_size.o \
	emile_map_open.o emile_map_close.o emile_map_read.o \
	emile_map_write.o emile_map_is_valid.o emile_map_geometry.o \
	emile_map_get_driver_number.o emile_map_get_driver_info.o \
	emile_map_bootblock_read.o emile_map_bootblock_write.o \
	emile_map_partition_is_valid.o emile_map_get_partition_geometry.o \
	emile_map_get_partition_type.o emile_map_get_partition_name.o \
	emile_map_partition_is_bootable.o emile_map_partition_is_startup.o \
	emile_map_set_partition_type.o emile_map_set_partition_name.o \
	emile_map_partition_set_bootable.o emile_map_partition_set_startup.o \
	emile_map_bootblock_is_valid.o emile_map_get_number.o \
	emile_map_bootblock_get_type.o emile_scsi_get_dev.o \
	emile_map_partition_get_flags.o emile_map_set_startup.o \
	emile_scsi_get_rdev.o emile_map_has_apple_driver.o \
	emile_map_seek_driver_partition.o emile_get_uncompressed_size.o \
	emile_map_partition_set_flags.o emile_checksum.o \
	emile_map_get_bootinfo.o emile_map_dev.o emile_map_set_bootinfo.o \
	emile_map_set_driver_info.o emile_map_set_driver_number.o \
	emile_block0_write.o

HEADERS = emile.h libemile.h emile-first.h

CFLAGS  = -Wall -g

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rc $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY)
