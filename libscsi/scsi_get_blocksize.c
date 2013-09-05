/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libscsi.h"

int scsi_get_blocksize(scsi_device_t *device)
{
	return device->sector_size;
}
