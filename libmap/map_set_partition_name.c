/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <string.h>

#include "libmap.h"

int map_set_partition_name(map_t *map, char* name)
{
	if (!map_partition_is_valid(map))
		return -1;

	if (strlen(name) > 31)
		return -1;

	strncpy(map->partition.PartName, name, 32);

	return 0;
}
