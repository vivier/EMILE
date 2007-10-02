/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_partition_set_bootable(map_t *map, int enable)
{
	if (!map_partition_is_valid(map))
		return -1;

	if (enable)
		map_partition_set_flags(map,
				kPartitionAUXIsValid | kPartitionAUXIsAllocated | 
				kPartitionAUXIsInUse | kPartitionAUXIsReadable | 
				kPartitionAUXIsWriteable | kPartitionIsMountedAtStartup | 0x80);
	else
		map_partition_set_flags(map, 
			map_partition_get_flags(map) & ~kPartitionIsMountedAtStartup);

	return 0;
}
