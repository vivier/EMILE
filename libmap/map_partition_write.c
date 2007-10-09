/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "libmap.h"

int map_partition_write(map_t* map, off_t offset, size_t size, char* sector)
{
	int ret;

	if (!map_partition_is_valid(map))
		return -1;

	offset += read_long((u_int32_t*)&map->partition.PyPartStart)
			* FLOPPY_SECTOR_SIZE;

	ret = map->device->write_sector(map->device, offset, sector, size);

	return ret;
}
