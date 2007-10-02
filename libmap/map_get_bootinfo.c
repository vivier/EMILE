/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libmap.h"

int map_get_bootinfo(map_t *map, int* bootstart, int *bootsize,
		     int *bootaddr, int *bootentry, int* checksum,
		     char* processor)
{
	 if (!map_is_valid(map))
		 return -1;

	*bootstart = read_long((u_int32_t*)&map->partition.LgBootStart);
	*bootsize = read_long((u_int32_t*)&map->partition.BootSize);
	*bootaddr = read_long((u_int32_t*)&map->partition.BootAddr);
	*bootentry = read_long((u_int32_t*)&map->partition.BootEntry);
	*checksum = read_long((u_int32_t*)&map->partition.BootCksum);
	strcpy(processor, map->partition.Processor);

	return 0;
}
