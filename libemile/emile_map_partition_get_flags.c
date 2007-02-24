static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_partition_get_flags(emile_map_t *map)
{
	return read_long((u_int32_t*)&map->partition.PartStatus);
}
