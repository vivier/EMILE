#include <zlib.h>

#include "libstream.h"

extern gzFile gzopen (filesystem_io_t *fs);
int stream_uncompress(stream_t *stream)
{
	gzFile *gz;

	gz = gzopen(&stream->fs);
	if (gz == NULL)
		return -1;

	stream->fs.volume = NULL;
	stream->fs.file = gz;
	stream->fs.read = (stream_read_t)gzread;
	stream->fs.lseek = (stream_lseek_t)gzseek;
	stream->fs.close = (stream_close_t)gzclose;
	stream->fs.umount = NULL;

	return 0;
}
