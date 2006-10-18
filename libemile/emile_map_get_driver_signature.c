static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>

#include "partition.h"
#include "libemile.h"
#include "emile.h"

unsigned long emile_map_get_driver_signature(emile_map_t* map)
{
	if (!emile_map_partition_is_valid(map))
		return 0;

	return read_long((u_int32_t*)map->partition.Pad);
}
