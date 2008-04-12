/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __LIBEXT2_H__
#define __LIBEXT2_H__

#include <unistd.h>
#include <linux/ext2_fs.h>

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

extern ext2_VOLUME* ext2_mount(device_io_t *device);
extern int ext2_umount(ext2_VOLUME *volume);
extern ext2_DIR* ext2_opendir(ext2_VOLUME *, char *name);
extern struct ext2_dir_entry_2* ext2_readdir(ext2_DIR* dir);
extern void ext2_closedir(ext2_DIR *dir);
extern ext2_FILE* ext2_open(ext2_VOLUME *, char* pathname);
extern ssize_t ext2_read(ext2_FILE *file, void *buf, size_t count);
extern void ext2_close(ext2_FILE *file);
extern int ext2_lseek(ext2_FILE *file, long offset, int whence);
extern int ext2_fstat(ext2_FILE *file, struct stream_stat *buf);

#endif /* __LIBEXT2_H__ */
