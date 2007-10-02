/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_set_driver_info(map_t *map, int number,
			      int block, int size, int type)
{
	if (!map_is_valid(map))
		return -1;

	if (number >= DD_MAX_DRIVER)
		return -1;

	write_long((u_int32_t*)&map->drivers.DrvInfo[number].Block, block);
	write_short((u_int16_t*)&map->drivers.DrvInfo[number].Size, size);
	write_short((u_int16_t*)&map->drivers.DrvInfo[number].Type, type);

	return 0;
}
