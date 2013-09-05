/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "libmap.h"

int map_read(map_t *map, int part)
{
	int ret;
	int blocksize = map->device->get_blocksize(map->device->data);

	if (map->current == part)
		return part;

	if (part > read_long((u_int32_t*)&map->partition.MapBlkCnt))
		return -1;

	ret = map->device->read_sector(map->device->data, 1 + part,
				       &map->partition, blocksize);
	if (ret == -1)
		return -1;

	map->current = part;

	return part;
}
