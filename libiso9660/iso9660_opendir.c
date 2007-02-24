/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <string.h>

#include "libiso9660.h"

static iso9660_DIR* iso9660_opendir_node(iso9660_VOLUME *volume, struct iso_directory_record *node)
{
	iso9660_DIR *dir;

	dir = (iso9660_DIR*)malloc(sizeof(iso9660_DIR));
	if (dir == NULL)
		return NULL;

	dir->extent = isonum_733((char *)node->extent);
	dir->len = isonum_733((char *)node->size);
	dir->index =  sizeof (dir->buffer);
	dir->volume = volume;

	return dir;
}

static struct iso_directory_record* idr_new(struct iso_directory_record* idr)
{
	struct iso_directory_record* result;
	int size = sizeof(*idr) + (int)idr->name_len[0];

	result = (struct iso_directory_record*)malloc(size);
	memcpy(result, idr, size);

	return result;
}

static struct iso_directory_record * seek_name(iso9660_VOLUME *volume,
					       struct iso_directory_record *idr,
					       char *name)
{
	struct iso_directory_record *result;
	char name_buf[256];
	iso9660_DIR *dir;

	dir = iso9660_opendir_node(volume, idr);
	if (dir == NULL)
		return NULL;

	while ((idr = iso9660_readdir(dir)) != NULL)
	{
		iso9660_name(volume->ucs_level, name_buf, idr);
		if (strcmp(name, name_buf) == 0)
		{
			result = idr_new(idr);
			iso9660_closedir(dir);
			return result;
		}
	}
	iso9660_closedir(dir);
	return NULL;
}

struct iso_directory_record* iso9660_get_node(
		iso9660_VOLUME *volume,
		struct iso_directory_record *dirnode,
		char *path)
{
	struct iso_directory_record* result;
	struct iso_directory_record* current;
	char name[256];
	int i;

	current = idr_new(dirnode);
	while(1)
	{
		/* ignore head '/' */

		while (*path && *path == '/')
			path++;

		if (*path == 0)
			break;

		/* extract first path component */

		i = 0;
		while (*path && *path != '/')
			name[i++] = *path++;
		name[i] = 0;

		/* seek first component in current directory */

		result = seek_name(volume, current, name);
		if (result == NULL)
			return NULL;
	
		free(current);
		current = result;
	}
	return current;
}

iso9660_DIR* iso9660_opendir(iso9660_VOLUME *volume, char *name)
{
	iso9660_DIR *dir;
	struct iso_directory_record *node;

	node = iso9660_get_root_node(volume);
	if (node == NULL)
		return NULL;

	node = iso9660_get_node(volume, node, name);
	if (node == NULL)
		return NULL;

	dir = iso9660_opendir_node(volume, node);

	free(node);

	dir->volume = volume;

	return dir;
}
