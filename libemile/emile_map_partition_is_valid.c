static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_partition_is_valid(emile_map_t *map)
{
	return read_short(&map->partition.Sig) == MAP_SIGNATURE;
}
