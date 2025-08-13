/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libext2.h"
#include "ext2.h"

int ext2_lseek(stream_FILE *_file, off_t offset, int whence)
{
        ext2_FILE *file = (ext2_FILE*)_file;
        long new_offset;

	switch(whence)
	{
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset = file->offset + offset;
		break;
	case SEEK_END:
		new_offset = file->inode->i_size + offset;
		break;
	default:
		return -1;
	}

	if ( (new_offset < 0) ||
	     (new_offset > file->inode->i_size) )
		return -1;

	file->offset = new_offset;

	return new_offset;
}
