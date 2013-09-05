/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "libmap.h"

int map_write_sector(map_t* map, off_t offset, char *buffer, size_t size)
{
	int ret;

	if (!map_partition_is_valid(map))
		return -1;

	offset += read_long((u_int32_t*)&map->partition.PyPartStart);

	ret = map->device->write_sector(map->device->data, offset, buffer, size);

	return ret;
}
