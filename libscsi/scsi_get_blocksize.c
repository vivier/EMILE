/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libscsi.h"

int scsi_get_blocksize(scsi_device_t *device)
{
	return device->sector_size;
}
