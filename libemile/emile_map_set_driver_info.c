static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_set_driver_info(emile_map_t *map, int number,
			      int block, int size, int type)
{
	if (!emile_map_is_valid(map))
		return -1;

	if (number >= DD_MAX_DRIVER)
		return -1;

	map->drivers.DrvInfo[number].Block = block;
	map->drivers.DrvInfo[number].Size = size;
	map->drivers.DrvInfo[number].Type = type;

	return 0;
}
