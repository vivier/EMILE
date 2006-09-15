static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_partition_set_startup(emile_map_t *map, int enable)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	if (enable)
		emile_map_partition_set_flags(map, 
				emile_map_partition_get_flags(map) | kPartitionIsStartup);
	else
		emile_map_partition_set_flags(map, 
				emile_map_partition_get_flags(map) & ~kPartitionIsStartup);

	return 0;
}
