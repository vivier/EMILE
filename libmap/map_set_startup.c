/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libmap.h"

int map_set_startup(char* dev_name, int partition)
{
	map_t* map;
	char *part_type;
	int ret;
	int i;

	map = map_open(dev_name, O_RDWR);
	if (map == NULL)
		return -1;

	/* check partition type */

	ret = map_read(map, partition);
	if (ret == -1)
		return -1;

	part_type = map_get_partition_type(map);
	if (strcmp(part_type, APPLE_HFS) != 0) {
		fprintf(stderr, 
		"ERROR: a startup partition must be of type Apple_HFS\n");
		return -1;
	}

	for (i = 0; i < map_get_number(map); i++)
	{
		ret = map_read(map, i);
		if (ret == -1)
			return -1;

		part_type = map_get_partition_type(map);
		if (strcmp(part_type, APPLE_HFS) == 0)
		{
			map_partition_set_bootable(map, i == partition);
			map_partition_set_startup(map, i == partition);
			ret = map_write(map, i);
			if (ret == -1)
				return -1;
		}
	}
	map_close(map);

	return 0;
}
