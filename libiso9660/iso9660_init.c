/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "libiso9660.h"

iso9660_read_t __iso9660_device_read;

void iso9660_init(iso9660_read_t func)
{
	__iso9660_device_read = func;
}
