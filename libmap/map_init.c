/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libmap.h"

int map_init(device_io_t *device, int partition)
{
	int ret;
	map_t *map;

	map = map_open(device);
	if (map == NULL)
		return -1;

	ret = map_read(map, partition);
	if (ret == -1)
	{
		map_close(map);
		return -1;
	}

	device->data = map;
	device->read_sector = (stream_read_sector_t)map_read_sector;
	device->close = (stream_close_t)map_close;
	device->get_blocksize = (stream_get_blocksize_t)map_get_blocksize;

	return 0;
}
