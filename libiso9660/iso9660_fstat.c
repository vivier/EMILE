/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libiso9660.h"

int iso9660_fstat(iso9660_FILE *file, struct stream_stat *buf)
{
	buf->st_size = file->size;

	return 0;
}
