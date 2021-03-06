/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libext2.h"
#include "ext2_utils.h"

static struct ext2_dir_entry_2 entry;

struct ext2_dir_entry_2 *ext2_readdir(stream_DIR *_dir)
{
	ext2_DIR *dir = (ext2_DIR*)_dir;
	int ret;

	ret = ext2_dir_entry(dir->volume, dir->inode, dir->index, &entry);
	if (ret == -1)
		return NULL;
	dir->index = ret;

	entry.name[entry.name_len] = 0;
	return &entry;
}
