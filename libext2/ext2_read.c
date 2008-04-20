/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libext2.h"
#include "ext2.h"
#include "ext2_utils.h"

size_t ext2_read(stream_FILE *_file, void *buf, size_t count)
{
	ext2_FILE *file = (ext2_FILE*)_file;
	int ret;

	ret = ext2_read_data(file->volume, file->inode, file->offset,
			     buf, count);
	if (ret == -1)
		return -1;
	file->offset += ret;
	return ret;
}
