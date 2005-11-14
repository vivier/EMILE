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

	if ( count > (file->size  - file->offset) )
		count = file->size  - file->offset;

	while (count > 0)
	{
		size_t part;
		int offset_extent;
		int offset_index;

		offset_extent = file->base + 
				    (file->offset / ISO9660_EXTENT_SIZE);
		offset_index = file->offset % ISO9660_EXTENT_SIZE;

		if (file->current != offset_extent)
		{
			if ( (offset_index == 0) && 
			     (count >= ISO9660_EXTENT_SIZE) )
			{
				/* direct i/o */

				int extents_nb;

				extents_nb = count / ISO9660_EXTENT_SIZE;

				part = extents_nb * ISO9660_EXTENT_SIZE;

				__iso9660_device_read(offset_extent, 
						      buf + read, part);
				file->offset += part;
				count -= part;
				read += part;

				continue;
			}

			file->current = offset_extent;
			__iso9660_device_read(offset_extent,
					    file->buffer, 
					    ISO9660_EXTENT_SIZE);
		}

		part = ISO9660_EXTENT_SIZE - offset_index;
		if (count < part)
			part = count;

		memcpy(buf + read, file->buffer + offset_index, part);

		file->offset += part;
		count -= part;
		read += part;
	}

	return read;
}
