/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef _LIBCONTAINER_H_
#define _LIBCONTAINER_H_

#include <sys/types.h>

#include <libstream.h>

struct emile_block {
	u_int32_t	offset;	/* offset of first block */
	u_int16_t	count;	/* number of blocks */
} __attribute__((packed));

struct emile_container {
	u_int32_t		size;
	struct emile_block	blocks[0];
} __attribute__((packed));

typedef struct {
	unsigned long offset;
	device_io_t *device;
	struct emile_container* container;
	unsigned long current_block;
	char *buffer[0];
} container_FILE;

extern container_FILE *container_open(device_io_t *device, char *current);
extern int container_close(container_FILE *file);
extern int container_lseek(container_FILE *file, off_t offset, int whence);
extern ssize_t container_read(container_FILE *file, void *ptr, size_t size);
extern int container_fstat(container_FILE *file, struct stream_stat *buf);

#endif /* _LIBCONTAINER_H_ */
