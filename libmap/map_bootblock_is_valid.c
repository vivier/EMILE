/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_bootblock_is_valid(char *bootblock)
{
	return (bootblock[0] == 0x4C) && (bootblock[1] == 0x4B);
}
