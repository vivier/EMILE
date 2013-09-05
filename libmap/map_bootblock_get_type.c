/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <string.h>

#include "libmap.h"

int map_bootblock_get_type(char* bootblock)
{
	if (!map_bootblock_is_valid(bootblock))
		return INVALID_BOOTBLOCK;

	if (strcmp(&bootblock[11], "System") == 0)
		return APPLE_BOOTBLOCK;

	if (strcmp(&bootblock[11], "Mac Bootloader") == 0)
		return EMILE_BOOTBLOCK;

	return UNKNOWN_BOOTBLOCK;
}
