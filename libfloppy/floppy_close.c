/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdlib.h>

#include "libfloppy.h"

int floppy_close(floppy_device_t* device)
{
	if (device == NULL)
		return -1;
	free(device);
	return 0;
}
