/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include "libmap.h"

void map_close(map_t *map)
{
	close(map->fd);
	free(map);
}
