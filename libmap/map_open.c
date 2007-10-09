/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "libmap.h"

map_t* map_open(device_io_t *device)
{
	map_t *map;
	int ret;

	ASSERT_DD(printf("INTERNAL ERROR: Bad Block 0 size structure\n");
		  return NULL;)
	ASSERT_P(printf("INTERNAL ERROR: Bad Partition size structure\n"); 
		 return NULL;)

	map = (map_t*)malloc(sizeof(map_t));
	if (map == NULL)
		return NULL;

	map->device = device;

	ret = device->read_sector(map->device, 0,
				  &map->drivers, sizeof(map->drivers));
	if (ret == -1)
	{
		free(map);
		return NULL;
	}

	ret = device->read_sector(map->device, sizeof(map->drivers),
			       &map->partition, sizeof(map->partition));
	if (ret == -1)
	{
		free(map);
		return NULL;
	}
	map->current = 0;
	
	return map;
}
