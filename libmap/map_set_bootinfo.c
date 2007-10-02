/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libmap.h"

int map_set_bootinfo(map_t *map, int bootstart, int bootsize,
		           int bootaddr, int bootentry, int checksum,
			   char* processor)
{
	 if (!map_is_valid(map))
		 return -1;

	write_long((u_int32_t*)&map->partition.LgBootStart, bootstart);
	write_long((u_int32_t*)&map->partition.BootSize, bootsize);
	write_long((u_int32_t*)&map->partition.BootAddr, bootaddr);
	write_long((u_int32_t*)&map->partition.BootEntry, bootentry);
	write_long((u_int32_t*)&map->partition.BootCksum, checksum);
	memset(map->partition.Processor, 0, sizeof(map->partition.Processor));
	strcpy(map->partition.Processor, processor);

	return 0;
}
