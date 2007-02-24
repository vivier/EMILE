static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_is_valid(emile_map_t *map)
{
	return read_short((u_int16_t*)&map->drivers.Sig) == DD_SIGNATURE;
}
