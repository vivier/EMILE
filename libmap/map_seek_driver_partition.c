/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_seek_driver_partition(map_t *map, int base)
{
	int start;
	int count;
	int i;
	int ret;

	for (i = 0; i < map_get_number(map); i++)
	{
		ret = map_read(map, i);
		if (ret == -1)
			return -1;

		map_get_partition_geometry(map, &start, &count);
		if (base == start)
			return i;
	}

	return -1;
}
