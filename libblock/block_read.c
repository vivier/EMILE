/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libblock.h"

size_t block_read(block_FILE *file, void *ptr, size_t size)
{
	int read = 0;
	int ret;
	int blocksize = file->device->get_blocksize(file->device->data);

	while (size != 0)
	{
		int part;
		int block_nb = (file->offset + file->base) / file->buffer_size;
		int block_offset = (file->offset + file->base) % file->buffer_size;

		if (block_nb != file->current)
		{
			ret = file->device->read_sector(file->device->data,
					(block_nb * file->buffer_size) / blocksize,
						 file->buffer, 
						 file->buffer_size);
			if (ret == -1)
				return read;
			file->current = block_nb;
		}

		part = file->buffer_size - block_offset;
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
