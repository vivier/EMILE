/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_get_driver_number(emile_map_t *map)
{
	if (!emile_map_is_valid(map))
		return -1;

	return map->drivers.DrvrCount;
}
