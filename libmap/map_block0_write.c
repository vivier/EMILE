/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "libmap.h"

int map_block0_write(map_t *map)
{
	int ret;

	ret = lseek(map->fd, 0, SEEK_SET);
	if (ret != 0)
		return -1;

	ret = write(map->fd, &map->drivers, sizeof(map->drivers));

	return ret;
}
