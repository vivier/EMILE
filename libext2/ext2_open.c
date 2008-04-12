/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libext2.h"
#include "ext2_utils.h"

ext2_FILE* ext2_open(ext2_VOLUME *volume, char* pathname)
{
	ext2_FILE *file;
	struct ext2_inode *inode;
	int ino;
	int ret;

	ino = ext2_seek_name(volume, pathname);
	if (ino == 0)
		return NULL;

	inode = (struct ext2_inode*)malloc(sizeof(struct ext2_inode));
	if (inode == NULL)
		return NULL;

	ret = ext2_get_inode(volume, ino, inode);
	if (ret == -1) {
		free(inode);
		return NULL;
	}

	file = (ext2_FILE*)malloc(sizeof(ext2_FILE));
	if (file == NULL) {
		free(inode);
		return NULL;
	}
	file->volume = volume;
	file->inode = inode;
	file->offset = 0;

	return file;
}
