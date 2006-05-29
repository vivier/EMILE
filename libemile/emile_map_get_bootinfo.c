static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <string.h>

#include "partition.h"
#include "libemile.h"
#include "emile.h"

int emile_map_get_bootinfo(emile_map_t *map, int* bootstart, int *bootsize,
		           int *bootaddr, int *bootentry, int* checksum,
			   char* processor)
{
	 if (!emile_map_is_valid(map))
		 return -1;

	*bootstart = read_long(&map->partition.LgBootStart);
	*bootsize = read_long(&map->partition.BootSize);
	*bootaddr = read_long(&map->partition.BootAddr);
	*bootentry = read_long(&map->partition.BootEntry);
	*checksum = read_long(&map->partition.BootCksum);
	strcpy(processor, map->partition.Processor);

	return 0;
}
