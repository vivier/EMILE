/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "libmap.h"

int map_write(map_t *map, int part)
{
	off_t offset;
	int ret;
	int blocksize = map->device->get_blocksize(map->device->data);

	if (part > map->partition.MapBlkCnt)
		return -1;

	offset = part * sizeof(struct Partition) + 
			sizeof(struct DriverDescriptor);

	ret = map->device->write_sector(map->device->data, offset / blocksize,  &map->partition,
					sizeof(struct Partition));
	if (ret != -1)
		return -1;

	map->current = part;

	return part;
}
