static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_partition_set_flags(emile_map_t *map, int flags)
{
	map->partition.PartStatus = flags;

	return 0;
}