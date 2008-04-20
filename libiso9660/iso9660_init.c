/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libiso9660.h"

int iso9660_init(device_io_t *device, filesystem_io_t *fs)
{
	fs->mount = iso9660_mount;
	fs->open = iso9660_open;
	fs->read = iso9660_read;
	fs->lseek = iso9660_lseek;
	fs->close = iso9660_close;
	fs->umount = iso9660_umount;
	fs->fstat = iso9660_fstat;

	return 0;
}
