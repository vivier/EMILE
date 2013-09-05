/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __EXT2_H__
#define __EXT2_H__

#include <unistd.h>
#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>

#include <libstream.h>

typedef struct ext2_VOLUME {
        device_io_t *device;
	struct ext2_super_block *super;
	unsigned int current;
	char *buffer;
} ext2_VOLUME;

typedef struct ext2_DIR {
        ext2_VOLUME *volume;
	struct ext2_inode *inode;
	off_t index;
} ext2_DIR;

typedef struct ext2_FILE {
        ext2_VOLUME *volume;
	struct ext2_inode *inode;
	off_t offset;
} ext2_FILE;
#endif /* __LIBEXT2_H__ */
