/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#include "libcontainer.h"

extern void error(char *x) __attribute__ ((noreturn));

static unsigned long seek_block(container_FILE *file)
{
	struct emile_container *container = file->container;
	ssize_t current;
	int i;
	unsigned long offset = file->offset;
	int block_size = file->device->get_blocksize(file->device->data);

	for (i = 0, current = 0;
	     container->blocks[i].offset != 0; i++)
	{
		int extent_size = block_size *
				  container->blocks[i].count;

		if ( (current <= offset) && (offset < current + extent_size) )
		{
			return container->blocks[i].offset + 
				(offset - current) / block_size;
		}

		current += extent_size;
	}

	return 0;
}

ssize_t container_read(container_FILE *file, void *ptr, size_t size)
{
	int err;
	ssize_t read = 0;
	int part;
	int block_size = file->device->get_blocksize(file->device->data);

	while (size != 0)
	{
		unsigned long block_nb;
		int block_offset;

		if (file->offset >= file->container->size)
			return read;

		block_nb = seek_block(file);
		block_offset = file->offset % block_size;

		if (block_nb == 0)
			error("BUG in libcontainer !!!");

		if (block_nb != file->current_block)
		{
			err = file->device->read_sector(
					file->device->data,
					block_nb,
					file->buffer,
					block_size);
			if (err == -1)
				return -1;
			file->current_block = block_nb;
		}

		part = block_size - block_offset;
		if (part > size)
			part = size;
		memcpy(ptr, file->buffer + block_offset, part);

		size -= part;
		ptr = (char*)ptr + part;
		file->offset += part;
		read += part;
	}

	return read;
}
