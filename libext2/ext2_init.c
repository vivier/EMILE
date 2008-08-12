/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "libext2.h"
#include "ext2.h"
#include "ext2_utils.h"

static int is_ext2(device_io_t *device)
{
	struct ext2_super_block super;
	ext2_get_super(device, &super);
	if (super.s_magic == EXT2_SUPER_MAGIC)
		return 1;
	return 0;
}

int ext2_init(device_io_t *device, filesystem_io_t *fs)
{
	if (!is_ext2(device))
		return -1;

	fs->mount = ext2_mount;
	fs->open = ext2_open;
	fs->read = ext2_read;
	fs->lseek = ext2_lseek;
	fs->close = ext2_close;
	fs->umount = ext2_umount;
	fs->fstat = ext2_fstat;

	return 0;
}
