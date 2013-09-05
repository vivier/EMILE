/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libblock.h"
#include "block.h"

int block_fstat(stream_FILE *_file, struct stream_stat *buf)
{
	block_FILE *file = (block_FILE*)_file;
	if (buf == NULL)
		return -1;
	if (file->size == -1)
		return -1;

	buf->st_size = file->size;

	return 0;
}
