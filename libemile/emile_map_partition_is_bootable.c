/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_partition_is_bootable(emile_map_t *map)
{
	return (map->partition.PartStatus & kPartitionAUXIsBootValid) == kPartitionAUXIsBootValid;
}
