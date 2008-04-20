/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libext2.h"

int ext2_init(device_io_t *device, filesystem_io_t *fs)
{
	fs->mount = ext2_mount;
	fs->open = ext2_open;
	fs->read = ext2_read;
	fs->lseek = ext2_lseek;
	fs->close = ext2_close;
	fs->umount = ext2_umount;
	fs->fstat = ext2_fstat;

	return 0;
}
