/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "libblock.h"

int block_fstat(block_FILE *file, struct stream_stat *buf)
{
	if (buf == NULL)
		return -1;
	if (file->size == -1)
		return -1;

	buf->st_size = file->size;

	return 0;
}
