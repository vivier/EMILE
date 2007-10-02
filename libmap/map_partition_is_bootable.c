/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_partition_is_bootable(map_t *map)
{
	return (map_partition_get_flags(map) & 
		kPartitionAUXIsBootValid) == kPartitionAUXIsBootValid;
}
