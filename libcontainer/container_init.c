/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libcontainer.h"

int container_init(device_io_t *device, filesystem_io_t *fs)
{
	fs->umount = container_umount;
	fs->open = container_open;
	fs->read = container_read;
	fs->lseek = container_lseek;
	fs->close = container_close;
	fs->fstat = container_fstat;

	return 0;
}
