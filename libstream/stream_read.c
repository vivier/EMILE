/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libstream.h"

int stream_read(stream_t *stream, void *buf, size_t count)
{
	return stream->fs.read(stream->file, buf, count);
}
