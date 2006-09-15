/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <string.h>

#include <macos/devices.h>

#include "libfloppy.h"

/* offset is a block number
 * size is the number of bytes to read
 */

int floppy_read_sector(floppy_device_t *device,
		       off_t offset, void* buffer, size_t size)
{
	OSErr err;
	ParamBlockRec_t param_block;

	/* check size to read is multiple of sector size */

	if (size & (SECTOR_SIZE - 1))
		return -1;

	memset(&param_block, 0, sizeof(param_block));

	param_block.ioBuffer = (unsigned long)buffer;
	param_block.ioVRefNum = device->unit + 1;
	param_block.ioRefNum = -5;
	param_block.ioReqCount = size;
	param_block.ioPosMode = fsFromStart;
	param_block.ioPosOffset = offset << SECTOR_SIZE_BITS;

	err = PBReadSync(&param_block);
	if (err != noErr)
		return -1;

	return 0;
}
