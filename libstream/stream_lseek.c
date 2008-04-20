/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libstream.h"

int stream_lseek(stream_t *stream, long offset, int whence)
{
	return stream->fs.lseek(stream->file, offset, whence);
}
