/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdlib.h>

#include "libscsi.h"

int scsi_close(scsi_device_t *device)
{
	if (device == NULL)
		return -1;
	free(device);
	return 0;
}
