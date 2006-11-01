/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <scsi/scsi.h>

#include <macos/errors.h>

#include "libscsi.h"

#define BUFFER_SIZE	(255)
scsi_device_t *scsi_open(int target)
{
	scsi_device_t *dev;
	unsigned char buff[BUFFER_SIZE];
	OSErr err;
	int retries;

	err = scsi_INQUIRY(target, buff, BUFFER_SIZE);
	if (err != noErr)
		printf("WARNING: cannot execute INQUIRY\n");

	retries = 0;
	while (retries < 300)
	{
		err = scsi_TEST_UNIT_READY(target);
		retries++;
		if (err == noErr) {
			err = scsi_REQUEST_SENSE(target, buff, BUFFER_SIZE);
			if ((err == noErr) && (buff[2] != UNIT_ATTENTION) &&
					      (buff[2] != NOT_READY))
				break;
		}
	}
	if (retries == 300)
	{
		printf("ERROR: unit not ready !\n");
		return NULL;
	}

	retries = 0;
	while (retries < 3)
	{
		err = scsi_READ_CAPACITY(target, buff, BUFFER_SIZE);
		if (err == noErr)
			break;
		retries++;
	}
	if (err != noErr) {
		printf("ERROR: cannot execute READ_CAPACITY\n");
		return NULL;
	}

	dev = (scsi_device_t *)malloc(sizeof(scsi_device_t));
	if (dev == NULL)
		return NULL;
	dev->target = target;
	dev->capacity = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
	dev->sector_size = (buff[4] << 24) | (buff[5] << 16) | (buff[6] << 8) | buff[7];

	if ((dev->sector_size != 512) && (dev->sector_size != 2048))
	{
		dev->sector_size = 2048;
	}
	return dev;
}
