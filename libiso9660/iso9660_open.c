/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>

#include "libiso9660.h"

iso9660_FILE* iso9660_open(char* pathname)
{
	struct iso_directory_record *root;
	struct iso_directory_record *idr;
	iso9660_FILE *file;

	root = iso9660_get_root_node();
	if (root == NULL)
		return NULL;

	idr = iso9660_get_node(root, pathname);
	if (idr == NULL)
		return NULL;

	file = (iso9660_FILE*)malloc(sizeof(iso9660_FILE));
	if (file == NULL)
		return NULL;

	file->base = isonum_733((unsigned char *)idr->extent);
	file->size = isonum_733((unsigned char *)idr->size);
	file->offset = 0;
	file->current = -1;

	free(idr);

	return file;
}
