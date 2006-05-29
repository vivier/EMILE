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

int emile_map_set_bootinfo(emile_map_t *map, int bootstart, int bootsize,
		           int bootaddr, int bootentry, int checksum,
			   char* processor)
{
	 if (!emile_map_is_valid(map))
		 return -1;

	write_long(&map->partition.LgBootStart, bootstart);
	write_long(&map->partition.BootSize, bootsize);
	write_long(&map->partition.BootAddr, bootaddr);
	write_long(&map->partition.BootEntry, bootentry);
	write_long(&map->partition.BootCksum, checksum);
	memset(map->partition.Processor, 0, sizeof(map->partition.Processor));
	strcpy(map->partition.Processor, processor);

	return 0;
}
