static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "partition.h"
#include "libemile.h"

int emile_block0_write(emile_map_t *map)
{
	int ret;

	ret = lseek(map->fd, 0, SEEK_SET);
	if (ret != 0)
		return -1;

	ret = write(map->fd, &map->drivers, sizeof(map->drivers));

	return ret;
}
