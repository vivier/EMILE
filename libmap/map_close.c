/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include "libmap.h"

void map_close(map_t *map)
{
	map->device->close((stream_FILE*)map->device);
	free(map->device);
	free(map);
}
