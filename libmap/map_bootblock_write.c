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

int map_bootblock_write(map_t* map, char* bootblock)
{
	off_t offset;
	int ret;

	if (!map_partition_is_valid(map))
		return -1;

	offset = read_long((u_int32_t*)&map->partition.PyPartStart) * 512;

	ret = map->device->write_sector(map->device,
					offset, bootblock, BOOTBLOCK_SIZE);

	return ret;
}
