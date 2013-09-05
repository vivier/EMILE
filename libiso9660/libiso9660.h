/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __LIBISO9660_H__
#define __LIBISO9660_H__

#include <unistd.h>
#include <linux/iso_fs.h>

#include <libstream.h>

extern int iso9660_init(device_io_t *device, filesystem_io_t *fs);
extern stream_VOLUME* iso9660_mount(device_io_t *device);
extern int iso9660_umount(stream_VOLUME *volume);
extern stream_DIR* iso9660_opendir(stream_VOLUME *, char *name);
extern stream_FILE* iso9660_open(stream_VOLUME *, char* pathname);
extern int iso9660_closedir(stream_DIR *dir);
extern struct iso_directory_record *iso9660_readdir(stream_DIR *dir);
extern int iso9660_is_directory(struct iso_directory_record * idr);
extern size_t iso9660_read(stream_FILE *file, void *buf, size_t count);
extern void iso9660_close(stream_FILE *file);
extern int iso9660_lseek(stream_FILE *file, long offset, int whence);
extern int iso9660_fstat(stream_FILE *file, struct stream_stat *buf);
extern void iso9660_name(stream_VOLUME *volume, struct iso_directory_record * idr, char *buffer);

#endif /* __LIBISO9660_H__ */
