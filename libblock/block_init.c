/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libblock.h"

int block_init(device_io_t *device, filesystem_io_t *fs)
{
	fs->mount = block_mount;
	fs->umount = block_umount;
	fs->open = block_open;
	fs->read = block_read;
	fs->lseek = block_lseek;
	fs->close = block_close;
	fs->fstat = block_fstat;

	return 0;
}
