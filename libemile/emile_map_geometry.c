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

	*block_size = read_short((u_int16_t*)&map->drivers.BlkSize);
	*block_count = read_long((u_int32_t*)&map->drivers.BlkCount);

	return 0;
}
