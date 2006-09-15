/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libstream.h"

int stream_read(stream_t *stream, void *buf, size_t count)
{
	return stream->fs.read(stream->fs.file, buf, count);
}
