/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "libmap.h"

int map_bootblock_read(map_t* map, char* bootblock)
{
	return map_partition_read(map, 0, BOOTBLOCK_SIZE / FLOPPY_SECTOR_SIZE, bootblock);
}
