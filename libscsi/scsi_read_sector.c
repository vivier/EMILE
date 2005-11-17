/*
 *
 * (c) 2004, 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <string.h>

#include <macos/devices.h>

#include "libscsi.h"

#define SECTOR_SIZE     (2048)
#define ISO_BLOCKS(X)   (((X) / SECTOR_SIZE) + (((X)%SECTOR_SIZE)?1:0))

/* offset is a block number
 * size is the number of bytes to read
 */

int scsi_read_sector(off_t offset, void* buffer, size_t size)
{
	OSErr err;

	err = scsi_READ(3, offset, ISO_BLOCKS(size), buffer, size);
	if (err != noErr)
		return -1;

	return 0;
}
