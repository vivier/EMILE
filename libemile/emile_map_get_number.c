static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include "partition.h"
#include "libemile.h"

int emile_map_get_number(emile_map_t *map)
{
	return map->partition.MapBlkCnt;
}
