#include <stdlib.h>

#include "libmap.h"

char *map_read_driver(map_t *map, int driver_number)
{
	int block, size;
	int blocksize;
	char *driver;
	int ret;

	if (!map_is_valid(map))
		return NULL;

	blocksize = map_get_blocksize(map);

	if (driver_number > map_get_driver_number(map))
		return NULL;

	block = read_long((u_int32_t*)&map->drivers.DrvInfo[driver_number].Block);
	size = read_short((u_int16_t*)&map->drivers.DrvInfo[driver_number].Size);

	driver = malloc(size * blocksize);
	if (driver == NULL)
		return NULL;

	ret = map->device->read_sector(map->device->data, block, driver, size * blocksize);
	if (ret == -1) {
		free(driver);
		driver = NULL;
	}

	return driver;
}
