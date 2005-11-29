/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
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
	u_int16_t		unit_id;
	u_int32_t		size;
	struct emile_block	blocks[0];
} __attribute__((packed));

typedef struct {
	unsigned long size;
	unsigned long offset;
	device_io_t device;
	struct emile_container* container;
	unsigned long last_current;
	unsigned long last_index;
	unsigned long current_block;
	char *buffer[0];
} container_FILE;
#endif /* _LIBCONTAINER_H_ */
