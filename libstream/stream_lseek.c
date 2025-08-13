/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libstream.h"

int stream_lseek(stream_t *stream, off_t offset, int whence)
{
        return stream->fs.lseek(stream->file, offset, whence);
}
