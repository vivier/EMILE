/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>

#include "libscsi.h"

#define SECTOR_SIZE	(2048)

scsi_device_t *scsi_open(int target)
{
	scsi_device_t *dev;

	dev = (scsi_device_t *)malloc(sizeof(scsi_device_t));
	if (dev == NULL)
		return NULL;
	dev->target = target;
	dev->sector_size = SECTOR_SIZE;

	return dev;
}
