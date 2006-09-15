static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_bootblock_is_valid(char *bootblock)
{
	return (bootblock[0] == 0x4C) && (bootblock[1] == 0x4B);
}
