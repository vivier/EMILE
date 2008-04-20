/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include <libstream.h>

extern int block_init(device_io_t *device, filesystem_io_t *fs);
extern stream_VOLUME *block_mount(device_io_t *device);
extern int block_umount(stream_VOLUME *volume);
extern stream_FILE *block_open(stream_VOLUME *volume, char *path);
extern void block_close(stream_FILE *file);
extern size_t block_read(stream_FILE *file, void *ptr, size_t size);
extern int block_lseek(stream_FILE *file, off_t offset, int whence);
extern int block_fstat(stream_FILE *file, struct stream_stat *buf);
