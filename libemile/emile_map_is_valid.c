/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_is_valid(emile_map_t *map)
{
	return map->drivers.Sig == DD_SIGNATURE;
}
