/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "partition.h"
#include "libemile.h"

int emile_map_read(emile_map_t *map, int part)
{
	off_t offset;
	int ret;

	if (map->current == part)
		return part;

	if (part > map->partition.MapBlkCnt)
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