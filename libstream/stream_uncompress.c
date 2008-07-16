#include <zlib.h>

#include "libstream.h"

extern gzFile gzopen (stream_t *stream);

int stream_uncompress(stream_t *stream)
{
	gzFile *gz;

	gz = gzopen(stream);
	if (gz == NULL)
		return -1;

	stream->file = gz;
	stream->fs.read = (stream_read_t)gzread;
	stream->fs.lseek = (stream_lseek_t)gzseek;
	stream->fs.close = (stream_close_t)gzclose;

	return 0;
}
