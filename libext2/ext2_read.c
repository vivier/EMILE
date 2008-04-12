/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libext2.h"
#include "ext2_utils.h"

ssize_t ext2_read(ext2_FILE *file, void *buf, size_t count)
{
	int ret;

	ret = ext2_read_data(file->volume, file->inode, file->offset,
			     buf, count);
	if (ret == -1)
		return -1;
	file->offset += ret;
	return ret;
}
