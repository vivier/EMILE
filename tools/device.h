/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <libstream.h>

extern int device_sector_size;

extern long device_open(char *device, int flags);
extern void device_close(void *data);
extern int device_write_sector(void *data,off_t offset, void* buffer, size_t size);
extern int device_read_sector(void *data, off_t offset, void* buffer, size_t size);
extern int device_get_blocksize(void *data);
