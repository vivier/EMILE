static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libemile.h"

int emile_map_bootblock_get_type(char* bootblock)
{
	if (!emile_map_bootblock_is_valid(bootblock))
		return INVALID_BOOTBLOCK;

	if (strcmp(&bootblock[11], "System") == 0)
		return APPLE_BOOTBLOCK;

	if (strcmp(&bootblock[11], "Mac Bootloader") == 0)
		return EMILE_BOOTBLOCK;

	return UNKNOWN_BOOTBLOCK;
}
