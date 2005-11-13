/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>

#include "scsi.h"

#define SECTOR_SIZE     (2048)
#define ISO_BLOCKS(X)   (((X) / SECTOR_SIZE) + (((X)%SECTOR_SIZE)?1:0))

void device_read(off_t offset, void* buffer, size_t size)
{
	scsi_READ(3, offset, ISO_BLOCKS(size), buffer, size);
}
