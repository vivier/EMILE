/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_get_partition_name(emile_map_t *map, char** name)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	*name = map->partition.PartName;

	return 0;
}
