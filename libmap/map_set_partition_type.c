/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libmap.h"

int map_set_partition_type(map_t *map, char* type)
{
	if (!map_partition_is_valid(map))
		return -1;

	if (strlen(type) > 31)
		return -1;

	strncpy(map->partition.PartType, type, 32);

	return 0;
}
