/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libstream.h"

int stream_fstat(stream_t *stream, struct stream_stat *buf)
{
	if (stream->fs.fstat == NULL)
		return -1;

	return stream->fs.fstat(stream->fs.file, buf);
}
