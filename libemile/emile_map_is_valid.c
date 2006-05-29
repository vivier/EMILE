static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_is_valid(emile_map_t *map)
{
	return read_short(&map->drivers.Sig) == DD_SIGNATURE;
}
