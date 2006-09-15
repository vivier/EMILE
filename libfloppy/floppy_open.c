/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libfloppy.h"

floppy_device_t *floppy_open(int unit)
{
	floppy_device_t *dev;

	dev = (floppy_device_t *)malloc(sizeof(floppy_device_t));

	if (dev == NULL)
		return NULL;

	dev->unit = unit;

	return dev;
}
