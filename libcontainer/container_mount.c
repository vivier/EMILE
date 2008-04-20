/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <libstream.h>
#include "libcontainer.h"
#include "container.h"

stream_VOLUME *container_mount(device_io_t *device)
{
        return (stream_VOLUME*)device;
}

int container_umount(stream_VOLUME *volume)
{
	return 0;
}
