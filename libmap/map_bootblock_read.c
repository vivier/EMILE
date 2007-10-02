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

int map_bootblock_read(map_t* map, char* bootblock)
{
	off_t offset;
	int ret;
	int fd;

	if (!map_partition_is_valid(map))
		return -1;

	fd = open(map->name, O_RDONLY);
	if (fd == -1)
		return -1;
	offset = read_long((u_int32_t*)&map->partition.PyPartStart) * 512;
	lseek(fd, offset, SEEK_SET);

	ret = read(fd, bootblock, BOOTBLOCK_SIZE);

	close(fd);

	return ret;
}
