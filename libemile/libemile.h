/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef _LIBEMILE_H
#define _LIBEMILE_H

#undef USE_16BIT_CHECKSUM

#include <sys/types.h>
#include <sys/stat.h>

#include <libcontainer.h>

static __attribute__((used)) char* libemile_header = "$CVSHeader$";

#define SCSI_SUPPORT

#include "../second/head.h"

#define EMILE_FIRST_TUNE_DRIVE	0x0001
#define EMILE_FIRST_TUNE_OFFSET	0x0002
#define EMILE_FIRST_TUNE_SIZE	0x0004

#define FLOPPY_SECTOR_SIZE	512
#define FIRST_LEVEL_SIZE	(FLOPPY_SECTOR_SIZE * 2)
#define BOOTBLOCK_SIZE		(FLOPPY_SECTOR_SIZE * 2)

#define MAJOR_IDE0	3
#define MAJOR_LOOP	7
#define MAJOR_SD	8
#define MAJOR_IDE1	22

enum {
	EEMILE_CANNOT_READ_FIRST	= -2,
	EEMILE_UNKNOWN_FIRST		= -3,
	EEMILE_CANNOT_WRITE_FIRST	= -4,
	EEMILE_MALLOC_ERROR		= -5,
	EEMILE_CANNOT_OPEN_FILE		= -6,
	EEMILE_CANNOT_WRITE_SECOND	= -7,
	EEMILE_CANNOT_WRITE_KERNEL	= -8,
	EEMILE_CANNOT_WRITE_RAMDISK	= -9,
	EEMILE_CANNOT_WRITE_PAD		= -10,
	EEMILE_CANNOT_CREATE_IMAGE	= -11,
	EEMILE_MISSING_FIRST		= -12,
	EEMILE_MISSING_SECOND		= -13,
	EEMILE_CANNOT_READ_SECOND	= -14,
	EEMILE_INVALID_SECOND		= -15,
	EEMILE_CANNOT_READ_KERNEL	= -16,
};

enum {
	INVALID_BOOTBLOCK = -1,
	APPLE_BOOTBLOCK = 0,
	EMILE_BOOTBLOCK,
	UNKNOWN_BOOTBLOCK,
};

#ifndef _PARTITION_H
typedef void emile_map_t;
#endif

static inline unsigned long emile_file_get_size(char* file)
{
        struct stat result;

        stat(file, &result);

        return (result.st_size + FLOPPY_SECTOR_SIZE - 1)
                / FLOPPY_SECTOR_SIZE * FLOPPY_SECTOR_SIZE;
}

extern int emile_first_set_param(int fd, unsigned short tune_mask, 
				 int drive_num, int second_offset, 
				 int second_size);
extern int emile_first_get_param(int fd, int *drive_num, int *second_offset, 
				 int *second_size);
extern int emile_first_set_param_scsi(int fd, char *second_name);
extern int emile_first_set_param_scsi_extents(int fd,
				 int drive_num, int second_offset, 
				 int second_size);
struct emile_container *emile_second_create_mapfile(short *unit_id, char *mapfile, char* kernel);
extern int emile_is_url(char *path);
extern int emile_floppy_create_image(char* first_level, char* second_level, 
				     char* kernel_image, char* ramdisk, 
				     char* image);
extern int emile_scsi_create_container(int fd, short *unit_id,
				       struct emile_container* container, int maxbloks);
extern emile_map_t* emile_map_open(char* dev, int flags);
extern void emile_map_close(emile_map_t *map);
extern int emile_map_get_number(emile_map_t *map);
extern int emile_map_read(emile_map_t *map, int part);
extern int emile_map_write(emile_map_t *map, int part);
extern int emile_map_partition_is_valid(emile_map_t *map);
extern int emile_map_get_partition_geometry(emile_map_t *map, int *start, int *count);
extern char* emile_map_get_partition_type(emile_map_t *map);
extern char* emile_map_get_partition_name(emile_map_t *map);
extern int emile_map_partition_is_bootable(emile_map_t *map);
extern int emile_map_partition_is_startup(emile_map_t *map);
extern int emile_map_set_partition_type(emile_map_t *map, char* type);
extern int emile_map_set_partition_name(emile_map_t *map, char* name);
extern int emile_map_partition_set_bootable(emile_map_t *map, int enable);
extern int emile_map_partition_set_startup(emile_map_t *map, int enable);
extern int emile_map_is_valid(emile_map_t *map);
extern int emile_map_partition_get_flags(emile_map_t *map);
extern int emile_map_partition_set_flags(emile_map_t *map, int flags);
extern int emile_map_geometry(emile_map_t *map, int *block_size, 
			      int *block_count);
extern int emile_map_get_driver_number(emile_map_t *map);
extern int emile_map_get_driver_info(emile_map_t *map, int number,
                              int *block, int *size, int* type);
extern int emile_map_bootblock_read(emile_map_t* map, char* bootblock);
extern int emile_map_bootblock_write(emile_map_t* map, char* bootblock);
extern int emile_map_bootblock_get_type(char* bootblock);
extern int emile_map_bootblock_is_valid(char *bootblock);
extern int emile_scsi_get_dev(int fd, int* driver, int *disk, int *partition);
extern int emile_get_dev_name(char *s, int driver, int disk, int partition);
extern int emile_map_set_startup(char* dev_name, int partition);
extern int emile_scsi_get_rdev(char* dev_name, int* driver, int *disk, int *partition);
extern int emile_is_apple_driver(emile_map_t *map);
extern int emile_map_has_apple_driver(emile_map_t *map);
extern int emile_map_seek_driver_partition(emile_map_t *map, int start);
extern int emile_get_uncompressed_size(char *file);
extern unsigned short emile_checksum(unsigned char *addr, unsigned int length);
extern unsigned short emile_checksum_ATA(unsigned char *addr, unsigned int length);
extern int emile_map_get_bootinfo(emile_map_t* map, int* bootstart, int *bootsize, int *bootaddr, int *bootentry, int* checksum, char* processor);
extern char* emile_map_dev(emile_map_t *map);
extern int emile_map_set_bootinfo(emile_map_t *map, int bootstart, int bootsize, int bootaddr, int bootentry, int checksum, char* processor);
extern int emile_map_set_driver_info(emile_map_t *map, int number, int block, int size, int type);
extern int emile_map_set_driver_number(emile_map_t *map, int number);
extern int emile_block0_write(emile_map_t *map);
extern char* emile_second_get_configuration(int fd);
extern int emile_second_set_configuration(int fd, char *configuration);
extern int emile_second_get_next_property(char *configuration, int index, char *name, char *property);
extern int emile_second_get_property(char *configuration, char *name, char *property);
extern void emile_second_set_property(char *configuration, char *name, char *property);
extern void emile_second_remove_property(char *configuration, char *name);
extern int emile_second_set_param(int fd, char *kernel, char *parameters, char *initrd);
extern int emile_second_get_param(int fd, char *kernel, char *parameters, char *initrd);
#endif
