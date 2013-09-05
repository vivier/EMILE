/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "libmap.h"

int map_block0_write(map_t *map)
{
	int ret;

	ret = map->device->write_sector(map->device->data, 0,
					&map->drivers, sizeof(map->drivers));

	return ret;
}
