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

map_t* map_open(char *dev, int flags)
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

	map->fd = open(dev, flags);
	if (map->fd == -1)
	{
		free(map);
		return NULL;
	}
	strncpy(map->name, dev, MAP_NAME_LEN);
	map->name[MAP_NAME_LEN - 1] = 0;

	ret = read(map->fd, &map->drivers, sizeof(map->drivers));
	if (ret == -1)
	{
		free(map);
		return NULL;
	}

	ret = read(map->fd, &map->partition, sizeof(map->partition));
	if (ret == -1)
	{
		free(map);
		return NULL;
	}
	map->current = 0;
	
	return map;
}
