static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <string.h>

#include "partition.h"
#include "libemile.h"

int emile_map_set_partition_name(emile_map_t *map, char* name)
{
	if (!emile_map_partition_is_valid(map))
		return -1;

	if (strlen(name) > 31)
		return -1;

	strncpy(map->partition.PartName, name, 32);

	return 0;
}
