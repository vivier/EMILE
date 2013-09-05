/*
 *
 * (c) 2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libmap.h"

int map_get_blocksize(map_t *map)
{
	return map->device->get_blocksize(map->device->data);
}
