/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef _LIBEMILE_H
#define _LIBEMILE_H

static __attribute__((used)) char* libemile_header = "$CVSHeader$";

#define SCSI_SUPPORT

#include "../second/head.h"

#define EMILE_FIRST_TUNE_DRIVE	0x0001
#define EMILE_FIRST_TUNE_OFFSET	0x0002
#define EMILE_FIRST_TUNE_SIZE	0x0004

extern int emile_first_set_param(int fd, unsigned short tune_mask, 
				 int drive_num, int file_ref, 
				 int second_offset, int second_size);
extern int emile_first_get_param(int fd, int *drive_num, int *file_ref,
				 int *second_offset, int *second_size);
extern int emile_first_set_param_scsi(int fd, char *second_name);
extern int emile_second_get_output(int fd, unsigned int *console_mask,
				   unsigned int *bitrate0, int *datasize0,
				   int *parity0, int *stopbits0,
				   unsigned int *bitrate1, int *datasize1,
				   int *parity1, int *stopbits1,
				   int *gestaltid);
extern int emile_second_set_output(int fd,
				   unsigned int enable_mask, 
				   unsigned int disable_mask,
				   unsigned int bitrate0, int datasize0,
				   int parity0, int stopbits0,
				   unsigned int bitrate1, int datasize1,
				   int parity1, int stopbits1, int gestaltid);
extern int emile_second_set_cmdline(int fd, char* cmdline);
extern int emile_second_get_cmdline(int fd, char* cmdline);
extern int emile_second_set_kernel(int fd, char *kernel_image, 
				   unsigned int kernel_offset,
				   unsigned int buffer_size, char* ramdisk);
extern int emile_second_get_kernel(int fd, unsigned int *kernel_offset,
				   unsigned int *kernel_image_size,
				   unsigned int *buffer_size, 
				   unsigned int *ramdisk_offset,
				   unsigned int *ramdisk_size);
extern int emile_scsi_create_container(int fd, 
				       struct emile_container* container);
#endif
