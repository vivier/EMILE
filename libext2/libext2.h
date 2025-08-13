/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __LIBEXT2_H__
#define __LIBEXT2_H__

#include <libstream.h>

extern int ext2_init(device_io_t *device, filesystem_io_t *fs);
extern stream_VOLUME* ext2_mount(device_io_t *device);
extern int ext2_umount(stream_VOLUME *volume);
extern stream_DIR* ext2_opendir(stream_VOLUME *, char *name);
extern struct ext2_dir_entry_2* ext2_readdir(stream_DIR* dir);
extern void ext2_closedir(stream_DIR *dir);
extern stream_FILE* ext2_open(stream_VOLUME *, char* pathname);
extern size_t ext2_read(stream_FILE *file, void *buf, size_t count);
extern void ext2_close(stream_FILE *file);
extern int ext2_lseek(stream_FILE *file, off_t offset, int whence);
extern int ext2_fstat(stream_FILE *file, struct stream_stat *buf);

#endif /* __LIBEXT2_H__ */
