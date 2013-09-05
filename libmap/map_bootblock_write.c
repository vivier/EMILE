/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "libmap.h"

int map_bootblock_write(map_t* map, char* bootblock)
{
	return map_write_sector(map, 0, bootblock, BOOTBLOCK_SIZE);
}
