/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libmap.h"

int map_get_driver_number(map_t *map)
{
	if (!map_is_valid(map))
		return -1;

	return read_short((u_int16_t*)&map->drivers.DrvrCount);
}
