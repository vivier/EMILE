/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>

#include "libmap.h"

char* map_dev(map_t *map)
{
	if (!map_partition_is_valid(map))
		return NULL;

	return map->name;
}