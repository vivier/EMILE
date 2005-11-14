/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <string.h>
#include <stdio.h>

#include "libiso9660.h"

extern iso9660_read_t __iso9660_device_read;

ssize_t iso9660_read(iso9660_FILE *file, void *buf, size_t count)
{
	size_t read = 0;

	while (count > 0)
	{
		size_t part;

		/* direct i/o */

		if ( (file->index == sizeof (file->buffer)) && 
		     (count > sizeof (file->buffer)) )
		{
			int extents;

			if (file->len < count)
				extents = file->len / sizeof(file->buffer);
			else
				extents = count / sizeof (file->buffer);

			part = extents * sizeof (file->buffer);

			__iso9660_device_read(file->extent, 
					      buf + read, part);
			file->len -= part;
			file->extent += extents;
			count -= part;
			read += part;

			continue;
		}

		if (file->index == sizeof (file->buffer))
		{
			if (file->len <= 0)
				return read;
			__iso9660_device_read(file->extent, 
					    file->buffer, 
					    sizeof (file->buffer));
			file->len -= sizeof (file->buffer);
			file->extent++;
			file->index = 0;
		}

		if (count > sizeof(file->buffer) - file->index)
			part = sizeof(file->buffer) - file->index;
		else
			part = count;

		memcpy(buf + read, file->buffer + file->index, part);

		count -= part;
		file->index += part;
		read += part;
	}

	return read;
}
