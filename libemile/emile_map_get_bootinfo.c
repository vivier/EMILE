static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <string.h>

#include "partition.h"
#include "libemile.h"

int emile_map_get_bootinfo(emile_map_t *map, int* bootstart, int *bootsize,
		           int *bootaddr, int *bootentry, int* checksum,
			   char* processor)
{
	 if (!emile_map_is_valid(map))
		 return -1;

	*bootstart = map->partition.LgBootStart;
	*bootsize = map->partition.BootSize;
	*bootaddr = map->partition.BootAddr;
	*bootentry = map->partition.BootEntry;
	*checksum = map->partition.BootCksum;
	strcpy(processor, map->partition.Processor);

	return 0;
}
