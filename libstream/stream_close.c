/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>

#include "libstream.h"

int stream_close(stream_t *stream)
{
	if (stream->fs.close(stream->fs.file) != 0)
		return -1;
	if (stream->fs.umount(stream->fs.volume) != 0)
		return -1;
	if (stream->device.close(stream->device.data) != 0)
		return -1;
	free(stream);
	return 0;
}
