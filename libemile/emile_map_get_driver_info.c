/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_get_driver_info(emile_map_t *map, int number,
			      int *block, int *size, int* type)
{
	if (!emile_map_is_valid(map))
		return -1;

	if (number > emile_map_get_driver_number(map))
		return -1;

	*block = map->drivers.DrvInfo[number].Block;
	*size = map->drivers.DrvInfo[number].Size;
	*type = map->drivers.DrvInfo[number].Type;

	return 0;
}
