/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_partition_is_startup(map_t *map)
{
	return (map_partition_get_flags(map)
		& kPartitionIsStartup) == kPartitionIsStartup;
}
