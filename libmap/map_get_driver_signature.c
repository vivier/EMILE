/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdio.h>

#include "libmap.h"

unsigned long map_get_driver_signature(map_t* map)
{
	if (!map_partition_is_valid(map))
		return 0;

	return read_long((u_int32_t*)map->partition.Pad);
}
