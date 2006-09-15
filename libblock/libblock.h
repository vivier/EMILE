/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include <libstream.h>

typedef struct {
	int base;
	int offset;
	int size;
	device_io_t *device;
	int current;
	int buffer_size;
	unsigned char buffer[0];
} block_FILE;

extern block_FILE *block_open(device_io_t *device, char *path);
extern int block_close(block_FILE *file);
extern size_t block_read(block_FILE *file, void *ptr, size_t size);
extern int block_lseek(block_FILE *file, off_t offset, int whence);
extern int block_fstat(block_FILE *file, struct stream_stat *buf);
