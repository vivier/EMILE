/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <libstream.h>

extern FILE *device_open(char *device);
extern void device_close(void *data);
extern int device_read_sector(void *data, off_t offset, void* buffer, size_t size);
