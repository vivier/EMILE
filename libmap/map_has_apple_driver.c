/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

/*
 * See http://developer.apple.com/technotes/tn/tn1189.html
 *
 */

#include <string.h>

#include "libmap.h"

int emile_is_apple_driver(map_t *map)
{
	return strncmp(map->partition.PartType, 
		       APPLE_DRIVER, strlen(APPLE_DRIVER)) == 0;
}

int map_has_apple_driver(map_t *map)
{
	int block, size, type, part;
	int i;
	int ret;

	for (i = 0; i < map_get_driver_number(map); i++)
	{
		map_get_driver_info(map, i, &block, &size, &type);

		part = map_seek_driver_partition(map, block);

		ret = map_read(map, part);
		if (ret == -1)
			return -1;

		if (emile_is_apple_driver(map))
			return 1;
	}

	return 0;
}
