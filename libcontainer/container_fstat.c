/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libcontainer.h"

int container_fstat(container_FILE *file, struct stream_stat *buf)
{
	if (buf == NULL)
		return -1;
	if (file->container->size == -1)
		return -1;

	buf->st_size = file->container->size;

	return 0;
}
