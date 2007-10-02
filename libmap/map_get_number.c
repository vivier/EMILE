/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include "libmap.h"

int map_get_number(map_t *map)
{
	return read_long((u_int32_t*)&map->partition.MapBlkCnt);
}
