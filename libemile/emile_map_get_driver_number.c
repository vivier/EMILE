static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_get_driver_number(emile_map_t *map)
{
	if (!emile_map_is_valid(map))
		return -1;

	return read_short(&map->drivers.DrvrCount);
}
