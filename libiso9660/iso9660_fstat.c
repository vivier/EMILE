/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libiso9660.h"
#include "iso9660.h"

int iso9660_fstat(stream_FILE *file, struct stream_stat *buf)
{
	buf->st_size = ((iso9660_FILE*)file)->size;
	buf->st_base = ((iso9660_FILE*)file)->base;

	return 0;
}
