static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_geometry(emile_map_t *map, int *block_size, int *block_count)
{
	if (!emile_map_is_valid(map))
		return -1;

	*block_size = map->drivers.BlkSize;
	*block_count = map->drivers.BlkCount;

	return 0;
}
