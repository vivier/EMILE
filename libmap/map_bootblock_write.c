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
	char name[MAP_NAME_LEN];
	int ret;
	int fd;

	if (!map_partition_is_valid(map))
		return -1;

	sprintf(name, "%s%d", map->name, map->current + 1);

	fd = open(name, O_WRONLY);
	if (fd == -1)
		return -1;

	ret = write(fd, bootblock, BOOTBLOCK_SIZE);

	close(fd);

	return ret;
}
