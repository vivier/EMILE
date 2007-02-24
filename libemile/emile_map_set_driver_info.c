static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_set_driver_info(emile_map_t *map, int number,
			      int block, int size, int type)
{
	if (!emile_map_is_valid(map))
		return -1;

	if (number >= DD_MAX_DRIVER)
		return -1;

	write_long((u_int32_t*)&map->drivers.DrvInfo[number].Block, block);
	write_short((u_int16_t*)&map->drivers.DrvInfo[number].Size, size);
	write_short((u_int16_t*)&map->drivers.DrvInfo[number].Type, type);

	return 0;
}
