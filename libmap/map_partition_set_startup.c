/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libmap.h"

int map_partition_set_startup(map_t *map, int enable)
{
	if (!map_partition_is_valid(map))
		return -1;

	if (enable)
		map_partition_set_flags(map, 
				map_partition_get_flags(map) | kPartitionIsStartup);
	else
		map_partition_set_flags(map, 
				map_partition_get_flags(map) & ~kPartitionIsStartup);

	return 0;
}
