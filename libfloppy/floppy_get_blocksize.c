#include "libfloppy.h"

int floppy_get_blocksize(floppy_device_t *device)
{
	return SECTOR_SIZE;
}
