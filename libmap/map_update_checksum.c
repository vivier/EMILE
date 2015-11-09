#include <stdlib.h>

#include "libmap.h"

int map_update_checksum(map_t *map, int driver_number)
{
	char *driver;
	int blocksize;
	int block;
	int part;
	int length, checksum;
	int ret;

	driver = map_read_driver(map, driver_number);
	if (driver == NULL)
		return -1;

	blocksize = map_get_blocksize(map);
	block = read_long((u_int32_t*)&map->drivers.DrvInfo[driver_number].Block);

	part = map_seek_driver_partition(map, block * blocksize / 512);

	map_read(map, part);

	length = read_long((u_int32_t*)&map->partition.BootSize);
	checksum = map_checksum((unsigned char*)driver, length);
	write_long((u_int32_t*)&map->partition.BootCksum, checksum);
	
	free(driver);

	ret = map_write(map, part);
	if (ret != part)
		return -1;

	return 0;
}
