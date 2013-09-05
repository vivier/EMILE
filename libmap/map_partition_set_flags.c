/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libmap.h"

int map_partition_set_flags(map_t *map, int flags)
{
	write_long((u_int32_t*)&map->partition.PartStatus, flags);

	return 0;
}
