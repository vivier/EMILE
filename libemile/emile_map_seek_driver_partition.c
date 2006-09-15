static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libemile.h"

int emile_map_seek_driver_partition(emile_map_t *map, int base)
{
	int start;
	int count;
	int i;
	int ret;

	for (i = 0; i < emile_map_get_number(map); i++)
	{
		ret = emile_map_read(map, i);
		if (ret == -1)
			return -1;

		emile_map_get_partition_geometry(map, &start, &count);
		if (base == start)
			return i;
	}

	return -1;
}
