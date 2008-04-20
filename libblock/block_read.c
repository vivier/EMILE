/*
 *
 * (c) 2005-2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libblock.h"
#include "block.h"

size_t block_read(stream_FILE *_file, void *ptr, size_t size)
{
	block_FILE *file = (block_FILE*)_file;
	int part;
	int block_nb;
	int block_offset;
	int read = 0;
	int ret;
	int blocksize = file->device->get_blocksize(file->device->data);

	while (size != 0)
	{
		block_nb = (file->base +
			    file->offset / blocksize) / file->buffer_size;
		block_offset = (file->base - block_nb * file->buffer_size) *
			       blocksize + file->offset;
		if (block_nb != file->current)
		{
			ret = file->device->read_sector(file->device->data,
						 block_nb * file->buffer_size,
						 file->buffer, 
						 file->buffer_size * blocksize);
			if (ret == -1)
				return read;
			file->current = block_nb;
		}

		part = file->buffer_size * blocksize - block_offset;
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
