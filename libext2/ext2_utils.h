/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __EXT2_UTILS_H__
#define __EXT2_UTILS_H__

#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>
#include <libstream.h>
#include "ext2.h"

extern void ext2_get_super(device_io_t *device, struct ext2_super_block *super);
extern void ext2_read_block(ext2_VOLUME* volume, unsigned int fsblock);
extern void ext2_get_group_desc(ext2_VOLUME* volume,
				int group_id, struct ext2_group_desc *gdp);
extern int ext2_get_inode(ext2_VOLUME* volume,
			  unsigned int ino, struct ext2_inode *inode);
extern unsigned int ext2_get_block_addr(ext2_VOLUME* volume,
					struct ext2_inode *inode,
					unsigned int logical);
extern int ext2_read_data(ext2_VOLUME* volume, struct ext2_inode *inode,
			  off_t offset, char *buffer, size_t length);
extern off_t ext2_dir_entry(ext2_VOLUME *volume, struct ext2_inode *inode,
			    off_t offset, struct ext2_dir_entry_2 *entry);
extern unsigned int ext2_seek_name(ext2_VOLUME *volume, char *name);
#endif /* __EXT2_UTILS_H__ */
