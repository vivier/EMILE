static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "partition.h"
#include "libemile.h"

int emile_map_set_startup(char* dev_name, int partition)
{
	emile_map_t* map;
	char *part_type;
	int ret;
	int i;

	map = emile_map_open(dev_name, O_RDWR);
	if (map == NULL)
		return -1;

	/* check partition type */

	ret = emile_map_read(map, partition);
	if (ret == -1)
		return -1;

	part_type = emile_map_get_partition_type(map);
	if (strcmp(part_type, APPLE_HFS) != 0) {
		fprintf(stderr, 
		"ERROR: a startup partition must be of type Apple_HFS\n");
		return -1;
	}

	for (i = 0; i < emile_map_get_number(map); i++)
	{
		ret = emile_map_read(map, i);
		if (ret == -1)
			return -1;

		part_type = emile_map_get_partition_type(map);
		if (strcmp(part_type, APPLE_HFS) == 0)
		{
			emile_map_partition_set_bootable(map, i == partition);
			emile_map_partition_set_startup(map, i == partition);
			ret = emile_map_write(map, i);
			if (ret == -1)
				return -1;
		}
	}
	emile_map_close(map);

	return 0;
}
