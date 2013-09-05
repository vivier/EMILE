/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libstream.h"

int stream_fstat(stream_t *stream, struct stream_stat *buf)
{
	int ret;

	if (stream->fs.fstat == NULL)
		return -1;

	ret = stream->fs.fstat(stream->file, buf);
	buf->st_dev = stream->unit;

	return ret;
}
