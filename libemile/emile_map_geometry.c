static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_geometry(emile_map_t *map, int *block_size, int *block_count)
{
	if (!emile_map_is_valid(map))
		return -1;

	*block_size = read_short(&map->drivers.BlkSize);
	*block_count = read_long(&map->drivers.BlkCount);

	return 0;
}
