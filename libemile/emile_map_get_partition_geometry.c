static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_get_partition_geometry(emile_map_t *map, int *start, int *count)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	*start = map->partition.PyPartStart;
	*count = map->partition.PartBlkCnt;

	return 0;
}
