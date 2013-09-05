/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libblock.h"
#include "block.h"

int block_lseek(stream_FILE *_file, off_t offset, int whence)
{
	block_FILE *file = (block_FILE*)_file;
	long new_offset;

	switch(whence)
	{
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset = file->offset + offset;
		break;
	default:
		return -1;
	}

	if (new_offset < 0)
		return -1;

	file->offset = new_offset;

	return new_offset;
}
