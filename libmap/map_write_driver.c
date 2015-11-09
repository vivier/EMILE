#include <stdlib.h>

#include "libmap.h"

/* the size information in DrvInfo must be the same as the driver size in memory */

int map_write_driver(map_t *map, int driver_number, char *driver)
{
	int block, size;
	int blocksize;
	int ret;

	if (driver == NULL)
		return -1;

	if (!map_is_valid(map))
		return -1;

	blocksize = map_get_blocksize(map);

	if (driver_number > map_get_driver_number(map))
		return -1;

	block = read_long((u_int32_t*)&map->drivers.DrvInfo[driver_number].Block);
	size = read_short((u_int16_t*)&map->drivers.DrvInfo[driver_number].Size);

	ret = map->device->write_sector(map->device->data, block, driver, size * blocksize);

	return ret;
}
