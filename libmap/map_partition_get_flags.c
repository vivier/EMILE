/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_partition_get_flags(map_t *map)
{
	return read_long((u_int32_t*)&map->partition.PartStatus);
}
