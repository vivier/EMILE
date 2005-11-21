/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "device.h"

#define SECTOR_SIZE     (2048)
#define ISO_BLOCKS(X)   (((X) / SECTOR_SIZE) + (((X)%SECTOR_SIZE)?1:0))

static const char *filename = "/dev/cdrom";

int device_read_sector(void *data,off_t offset, void* buffer, size_t size)
{
	FILE* file = (FILE*)data;

	lseek(fileno(file), offset << 11, SEEK_SET);
	return read(fileno(file), buffer, ISO_BLOCKS(size) << 11);
}

void device_close(void *data)
{
	FILE* file = (FILE*)data;
	if (file)
		fclose(file);
}

FILE *device_open(void)
{
	FILE* file;

	file = fopen(filename, "rb");
	if (file == NULL)
		return NULL;

	return file;
}
