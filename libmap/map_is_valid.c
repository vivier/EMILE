/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libmap.h"

int map_is_valid(map_t *map)
{
	return read_short((u_int16_t*)&map->drivers.Sig) == DD_SIGNATURE;
}
