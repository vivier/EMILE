/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libblock.h"

stream_VOLUME *block_mount(device_io_t *device)
{
	return (stream_VOLUME*)device;
}

int block_umount(stream_VOLUME *volume)
{
	return 0;
}
