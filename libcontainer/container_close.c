/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>

#include "libcontainer.h"

int container_close(container_FILE *file)
{
	if (file == NULL)
		return -1;

	if (file->container)
		free(file->container);

	free(file);

	return 0;
}
