static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_partition_set_bootable(emile_map_t *map, int enable)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	if (enable)
		emile_map_partition_set_flags(map,
				kPartitionAUXIsValid | kPartitionAUXIsAllocated | 
				kPartitionAUXIsInUse | kPartitionAUXIsReadable | 
				kPartitionAUXIsWriteable | kPartitionIsMountedAtStartup | 0x80);
	else
		emile_map_partition_set_flags(map, 
			emile_map_partition_get_flags(map) & ~kPartitionIsMountedAtStartup);

	return 0;
}
