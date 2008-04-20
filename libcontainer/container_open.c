/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>

#include <stdlib.h>

#include "libcontainer.h"
#include "container.h"

stream_FILE *container_open(stream_VOLUME *volume, char *path)
{
	container_FILE *file;
	device_io_t *device = (device_io_t*)volume;
	int block_size = device->get_blocksize(device->data);
	unsigned long first, nbblocs;
	int ret;

	first = strtol(path, &path, 0);
	if (*path != ',')
		return NULL;
	path++;
	nbblocs = strtol(path, &path, 0);
	if (*path != 0)
		return NULL;
	file = (container_FILE *)malloc(sizeof(container_FILE) + block_size);
	if (file == NULL)
		return NULL;

	file->container = (struct emile_container*)malloc(block_size * nbblocs);
	if (file->container == NULL)
	{
		free(file);
		return NULL;
	}

	ret = device->read_sector(device->data, first, file->container, block_size * nbblocs);
	if (ret == -1)
	{
		free(file->container);
		free(file);
	}

	file->offset = 0;
	file->device = device;
	file->current_block = 0;
	return (stream_FILE*)file;
}
