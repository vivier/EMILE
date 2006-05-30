static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "partition.h"
#include "libemile.h"

int emile_map_bootblock_read(emile_map_t* map, char* bootblock)
{
	char name[MAP_NAME_LEN];
	int ret;
	int fd;

	if (!emile_map_partition_is_valid(map))
		return -1;

	sprintf(name, "%s%d", map->name, map->current + 1);

	fd = open(name, O_RDONLY);
	if (fd == -1)
		return -1;

	ret = read(fd, bootblock, BOOTBLOCK_SIZE);

	close(fd);

	return ret;
}
