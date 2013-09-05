/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libext2.h"
#include "ext2.h"

int ext2_fstat(stream_FILE *file, struct stream_stat *buf)
{
	buf->st_size = ((ext2_FILE*)file)->inode->i_size;
	return 0;
}
