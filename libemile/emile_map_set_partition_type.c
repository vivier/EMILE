/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <string.h>

#include "partition.h"
#include "libemile.h"

int emile_map_set_partition_type(emile_map_t *map, char* type)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	if (strlen(type) > 31)
		return -1;

	strncpy(map->partition.PartType, type, 32);

	return 0;
}
