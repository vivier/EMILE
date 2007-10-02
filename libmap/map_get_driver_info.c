/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_get_driver_info(map_t *map, int number,
			      int *block, int *size, int* type)
{
	if (!map_is_valid(map))
		return -1;

	if (number > map_get_driver_number(map))
		return -1;

	*block = read_long((u_int32_t*)&map->drivers.DrvInfo[number].Block);
	*size = read_short((u_int16_t*)&map->drivers.DrvInfo[number].Size);
	*type = read_short((u_int16_t*)&map->drivers.DrvInfo[number].Type);

	return 0;
}
