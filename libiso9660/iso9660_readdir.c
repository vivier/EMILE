/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libiso9660.h"

struct iso_directory_record *iso9660_readdir(iso9660_DIR *dir)
{
	struct iso_directory_record *idr;

	if (dir->index > 2048 - offsetof(struct iso_directory_record, name[0]))
	{
		if (dir->len <= 0)
			return NULL;
		dir->volume->device->read_sector(dir->volume->device->data, dir->extent, dir->buffer, sizeof (dir->buffer));
		dir->len -= sizeof (dir->buffer);
		dir->extent++;
		dir->index = 0;
	}

	idr = (struct iso_directory_record *) &dir->buffer[dir->index];
	if (idr->length[0] == 0)
		return NULL;

	dir->index += dir->buffer[dir->index];

	return idr;
}
