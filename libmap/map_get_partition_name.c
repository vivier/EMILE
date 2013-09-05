/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdio.h>

#include "libmap.h"

char* map_get_partition_name(map_t *map)
{
	if (!map_partition_is_valid(map))
		return NULL;

	return map->partition.PartName;
}
