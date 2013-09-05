/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdlib.h>

#include "libcontainer.h"
#include "container.h"

void container_close(stream_FILE *file)
{
	if (file == NULL)
		return;

	if (((container_FILE*)file)->container)
		free(((container_FILE*)file)->container);

	free(file);

	return;
}
