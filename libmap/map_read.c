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

	ret = lseek(map->fd, offset, SEEK_SET);
	if (ret != offset)
		return -1;
	
	ret = read(map->fd, &map->partition, sizeof(struct Partition));
	if (ret != sizeof(struct Partition))
		return -1;

	map->current = part;

	return part;
}
