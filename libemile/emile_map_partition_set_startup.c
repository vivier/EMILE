static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_partition_set_startup(emile_map_t *map, int enable)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	if (enable)
		map->partition.PartStatus |= kPartitionIsStartup;
	else
		map->partition.PartStatus &= ~kPartitionIsStartup;

	return 0;
}
