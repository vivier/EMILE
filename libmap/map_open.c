/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
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
	int blocksize = device->get_blocksize(device->data);

	ASSERT_DD(printf("INTERNAL ERROR: Bad Block 0 size structure\n");
		  return NULL;)
	ASSERT_P(printf("INTERNAL ERROR: Bad Partition size structure\n"); 
		 return NULL;)
	
	if (blocksize > MAP_BLOCKSIZE)
	{
		printf("device block size too big (%d)\n", blocksize);
		return NULL;
	}

	map = (map_t*)malloc(sizeof(map_t));
	if (map == NULL)
		return NULL;

	map->device = malloc(sizeof(device_io_t));
	if (map->device == NULL)
	{
		free(map);
		return NULL;
	}
	memcpy(map->device, device, sizeof(device_io_t));

	ret = map->device->read_sector(map->device->data, 0,
				       &map->drivers, blocksize);
	if (ret == -1)
	{
		printf("Cannot read block 0\n");
		free(map);
		return NULL;
	}

	ret = map->device->read_sector(map->device->data, 1,
				       &map->partition, blocksize);
	if (ret == -1)
	{
		printf("Cannot read first partition descriptor\n");
		free(map);
		return NULL;
	}
	map->current = 0;
	
	return map;
}
