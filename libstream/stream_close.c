/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdlib.h>

#include "libstream.h"

int stream_close(stream_t *stream)
{
	if (stream->fs.close)
		stream->fs.close(stream->file);
	if (stream->fs.umount &&
	    stream->fs.umount(stream->volume) != 0)
		return -1;
	if (stream->device.close)
		stream->device.close(stream->device.data);
	free(stream);
	return 0;
}
