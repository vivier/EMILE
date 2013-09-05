/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <sys/types.h>

#include <libstream.h>

typedef struct {
	unsigned long offset;
	device_io_t *device;
	struct emile_container* container;
	unsigned long current_block;
	char *buffer[0];
} container_FILE;

typedef device_io_t container_VOLUME;

#endif /* _CONTAINER_H_ */
