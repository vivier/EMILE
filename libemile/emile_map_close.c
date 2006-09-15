static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include "partition.h"
#include "libemile.h"

void emile_map_close(emile_map_t *map)
{
	close(map->fd);
	free(map);
}
