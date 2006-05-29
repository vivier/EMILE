static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_get_partition_geometry(emile_map_t *map, int *start, int *count)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	*start = read_long(&map->partition.PyPartStart);
	*count = read_long(&map->partition.PartBlkCnt);

	return 0;
}
