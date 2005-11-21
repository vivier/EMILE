/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */
#ifndef __LIBSTREAM_H__
#define __LIBSTREAM_H__

#include <sys/types.h>
#include <unistd.h>

typedef int (*stream_read_sector_t)(void *data,off_t offset, void* buffer, size_t size);
typedef ssize_t (*stream_read_t)(void *data, void *buf, size_t count);
typedef int (*stream_seek_t)(void *data, long offset, int whence);
typedef int (*stream_close_t)(void *data);

typedef struct {
	void *data;
	stream_read_sector_t read_sector;
	stream_close_t close;
} device_io_t;

typedef struct {
	void *data;
	stream_read_t read;
	stream_seek_t seek;
	stream_close_t close;
} filesystem_io_t;

typedef struct {
	/* device interface */

	device_io_t device;

	/* filesystem interface */

	filesystem_io_t fs;
} stream_t;

extern stream_t *stream_open(char *dev);
extern int stream_read_sector(stream_t *stream,
			     off_t offset, void* buffer, size_t size);
extern int stream_close(stream_t *stream);
#endif /* __LIBSTREAM_H__ */
