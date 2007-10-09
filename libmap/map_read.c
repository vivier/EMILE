/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "libmap.h"

int map_read(map_t *map, int part)
{
	off_t offset;
	int ret;

	if (map->current == part)
		return part;

	if (part > read_long((u_int32_t*)&map->partition.MapBlkCnt))
		return -1;

	offset = part * sizeof(struct Partition) + sizeof(struct DriverDescriptor);

	ret = map->device->read_sector(map->device, offset, &map->partition,
				       sizeof(struct Partition));
	if (ret == -1)
		return -1;

	map->current = part;

	return part;
}
