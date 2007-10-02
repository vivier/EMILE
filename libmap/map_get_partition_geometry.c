/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_get_partition_geometry(map_t *map, int *start, int *count)
{
	if (!map_partition_is_valid(map))
		return -1;

	*start = read_long((u_int32_t*)&map->partition.PyPartStart);
	*count = read_long((u_int32_t*)&map->partition.PartBlkCnt);

	return 0;
}
