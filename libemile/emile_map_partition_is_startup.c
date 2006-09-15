static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_partition_is_startup(emile_map_t *map)
{
	return (emile_map_partition_get_flags(map)
		& kPartitionIsStartup) == kPartitionIsStartup;
}
