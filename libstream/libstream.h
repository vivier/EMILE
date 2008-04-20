/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */
#ifndef __LIBSTREAM_H__
#define __LIBSTREAM_H__

#include <sys/types.h>
#include <unistd.h>

typedef enum {
	device_FLOPPY,
	device_SCSI,
} device_t;

typedef enum {
	fs_EXT2,
	fs_ISO9660,
	fs_CONTAINER,
	fs_BLOCK,
	fs_LAST
} fs_t;

struct stream_stat {
	int st_dev;
	off_t st_size;
	off_t st_base;
};

typedef struct device_io_t device_io_t;
typedef struct filesystem_io_t filesystem_io_t;
typedef void *stream_FILE;
typedef void *stream_VOLUME;
typedef void *stream_DIR;
;
typedef int (*stream_read_sector_t)(void *data,off_t offset, void* buffer, size_t size);
typedef int (*stream_write_sector_t)(void *data,off_t offset, void* buffer, size_t size);
typedef int (*stream_get_blocksize_t)(void *data);

typedef stream_VOLUME *(*stream_mount_t)(device_io_t *device);
typedef int (*stream_init_t)(device_io_t *device, filesystem_io_t *fs);
typedef stream_FILE *(*stream_open_t)(stream_VOLUME *volume, char *path);
typedef int (*stream_umount_t)(stream_VOLUME *volume);

typedef size_t (*stream_read_t)(stream_FILE *file, void *buf, size_t count);
typedef int (*stream_lseek_t)(stream_FILE *file, long offset, int whence);
typedef void (*stream_close_t)(stream_FILE *file);
typedef int (*stream_fstat_t)(stream_FILE *file, struct stream_stat *buf);

struct device_io_t {
	void *data;
	stream_write_sector_t write_sector;
	stream_read_sector_t read_sector;
	stream_close_t close;
	stream_get_blocksize_t get_blocksize;
};

struct filesystem_io_t {
	stream_mount_t mount;
	stream_umount_t umount;
	stream_open_t open;
	stream_read_t read;
	stream_lseek_t lseek;
	stream_close_t close;
	stream_fstat_t fstat;
};

typedef struct {
	/* device interface */

	device_io_t device;

	/* filesystem interface */

	filesystem_io_t fs;

	/* info */

	void *volume;
	void *file;
	fs_t fs_id;
	device_t device_id;
	int unit, partition;

} stream_t;

extern stream_t *stream_open(char *dev);
extern int stream_read_sector(stream_t *stream,
			     off_t offset, void* buffer, size_t size);
extern int stream_read(stream_t *stream, void *buf, size_t count);
extern int stream_lseek(stream_t *stream, long offset, int whence);
extern int stream_close(stream_t *stream);
extern int stream_uncompress(stream_t *stream);
extern int stream_fstat(stream_t *stream, struct stream_stat *buf);
#endif /* __LIBSTREAM_H__ */
