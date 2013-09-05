/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
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

typedef device_io_t block_VOLUME;
