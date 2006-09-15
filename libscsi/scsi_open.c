/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include <macos/errors.h>

#include "libscsi.h"

scsi_device_t *scsi_open(int target)
{
	scsi_device_t *dev;
	unsigned char buff[8];
	OSErr err;

	err = scsi_READ_CAPACITY(target, buff, 8);
	if (err != noErr)
		return NULL;

	dev = (scsi_device_t *)malloc(sizeof(scsi_device_t));
	if (dev == NULL)
		return NULL;
	dev->target = target;
	dev->capacity = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
	dev->sector_size = (buff[4] << 24) | (buff[5] << 16) | (buff[6] << 8) | buff[7];

	return dev;
}
