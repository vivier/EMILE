/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libiso9660.h"
#include "iso9660.h"

stream_FILE* iso9660_open(stream_VOLUME *volume, char* pathname)
{
	struct iso_directory_record *root;
	struct iso_directory_record *idr;
	iso9660_FILE *file;

	root = iso9660_get_root_node((iso9660_VOLUME*)volume);
	if (root == NULL)
		return NULL;

	idr = iso9660_get_node((iso9660_VOLUME*)volume, root, pathname);
	if (idr == NULL)
		return NULL;

	file = (iso9660_FILE*)malloc(sizeof(iso9660_FILE));
	if (file == NULL)
		return NULL;

	file->base = isonum_733((char *)idr->extent);
	file->size = isonum_733((char *)idr->size);
	file->offset = 0;
	file->current = -1;
	file->volume = (iso9660_VOLUME*)volume;

	free(idr);

	return (stream_FILE*)file;
}
