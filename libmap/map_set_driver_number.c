/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libmap.h"

int map_set_driver_number(map_t *map, int number)
{
	if (!map_is_valid(map))
		return -1;

	if (number >= DD_MAX_DRIVER)
		return -1;

	write_short((u_int16_t*)&map->drivers.DrvrCount, number);

	return 0;
}
