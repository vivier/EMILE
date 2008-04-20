/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libblock.h"
#include "block.h"

#define NB_SECTORS	(18*2)

/*
 * path is "<first>,<size>"
 * where <first> is the offset of the first byte to read on the device
 * and <size> is the number of bytes to read then.
 */

stream_FILE *block_open(stream_VOLUME *volume, char *path)
{
	block_FILE *block;
	int blocksize = ((device_io_t*)volume)->get_blocksize(((device_io_t*)volume)->data);
	int first, size;

	first = strtol(path, &path, 0);
	if ( (*path != ',') && (*path != 0) )
		return NULL;

	if (*path == ',')
	{
		path++;
		size = strtol(path, &path, 0);
		if (*path != 0)
			return NULL;
	}
	else
		size = -1;
	
	block = (block_FILE *)malloc(sizeof(block_FILE) + 
				     NB_SECTORS * blocksize);
	if (block == NULL)
		return NULL;

	block->base = first;
	block->offset = 0;
	block->size = size;
	block->device = (device_io_t*)volume;
	block->current = -1;
	block->buffer_size = NB_SECTORS;

	return (stream_FILE*)block;
}
