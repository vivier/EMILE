/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "libstream.h"

int stream_lseek(stream_t *stream, long offset, int whence)
{
	return stream->fs.lseek(stream->fs.file, offset, whence);
}
