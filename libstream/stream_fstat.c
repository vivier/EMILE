/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "libstream.h"

int stream_fstat(stream_t *stream, struct stream_stat *buf)
{
	if (stream->fs.fstat == NULL)
		return -1;

	return stream->fs.fstat(stream->fs.file, buf);
}
