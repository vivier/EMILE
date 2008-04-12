/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libext2.h"

int ext2_fstat(ext2_FILE *file, struct stream_stat *buf)
{
	buf->st_size = file->inode->i_size;
	return 0;
}
