/*
 *
 * (c) 2005-2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libcontainer.h"
#include "container.h"

int container_fstat(stream_FILE *_file, struct stream_stat *buf)
{
	container_FILE *file = (container_FILE*)_file;
	if (buf == NULL)
		return -1;
	if (file->container->size == -1)
		return -1;

	buf->st_size = file->container->size;

	return 0;
}
