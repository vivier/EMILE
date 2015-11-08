/*
 *
 * (c) 2004-2007  Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef _LIBEMILE_H
#define _LIBEMILE_H

#undef USE_16BIT_CHECKSUM

#include <sys/types.h>
#include <sys/stat.h>

#include <libcontainer.h>

#define SCSI_SUPPORT

#include "../second/head.h"
#include "libmap.h"

#define EMILE_FIRST_TUNE_DRIVE	0x0001
#define EMILE_FIRST_TUNE_OFFSET	0x0002
#define EMILE_FIRST_TUNE_SIZE	0x0004

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
				 int second_size,
				 int blocksize);
struct emile_container *emile_second_create_mapfile(unsigned short *unit_id, char *mapfile, char* kernel);
extern int emile_is_url(char *path);
extern int emile_floppy_create(char *image, char* first_level, char* second_level);
extern char* emile_floppy_add(int fd, char *image);
extern int emile_floppy_close(int fd);
extern int emile_floppy_create_image(char* first_level, char* second_level, 
				     char* kernel_image, char* ramdisk, 
				     char* image);
extern int emile_scsi_create_container(int fd, unsigned short *unit_id,
				       struct emile_container* container, int maxbloks);
extern int8_t* emile_second_get_configuration(int fd);
extern int emile_second_set_configuration(int fd, int8_t *configuration);
extern int emile_second_get_next_property(int8_t *configuration, int index, char *name, char *property);
extern int emile_second_get_property(int8_t *configuration, char *name, char *property);
extern void emile_second_set_property(int8_t *configuration, char *name, char *property);
extern void emile_second_remove_property(int8_t *configuration, char *name);
extern int emile_second_set_param(int fd, char *kernel, char *parameters, char *initrd);
extern int emile_second_get_param(int fd, char *kernel, char *parameters, char *initrd);
extern int emile_scsi_get_dev(int fd, int* driver, int *disk, int *partition);
extern int emile_get_dev_name(char *buffer, int driver, int disk, int partition);
extern int emile_scsi_get_rdev(char* dev_name, int* driver, int *disk, int *partition);
#endif
