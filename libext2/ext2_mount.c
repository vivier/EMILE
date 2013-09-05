/*
 *
 * (c) 2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdlib.h>
#include <libstream.h>
#include "libext2.h"
#include "ext2.h"
#include "ext2_utils.h"

#define SB_OFFSET (2)

stream_VOLUME* ext2_mount(device_io_t *device)
{
	ext2_VOLUME *volume;
	struct ext2_super_block *super;
	char *buffer;

	super = (struct ext2_super_block*)malloc(sizeof(struct ext2_super_block));
	if (super == NULL)
		return NULL;

	ext2_get_super(device, super);
	if (super->s_magic != EXT2_SUPER_MAGIC) {
		free(super);
		return NULL;
	}

	buffer = (char*)malloc(EXT2_BLOCK_SIZE(super));
	if (buffer == NULL) {
		free(super);
		return NULL;
	}

	volume = (ext2_VOLUME*)malloc(sizeof(ext2_VOLUME));
	if (volume == NULL) {
		free(super);
		free(buffer);
		return NULL;
	}

	volume->buffer = buffer;
	volume->device = device;
	volume->super = super;

	volume->current = -1;
	ext2_read_block(volume, 0);

	return (stream_VOLUME*)volume;
}

int ext2_umount(stream_VOLUME* _volume)
{
	ext2_VOLUME *volume = (ext2_VOLUME*)_volume;
	if (volume == NULL)
		return -1;
	free(volume->super);
	free(volume->buffer);
	free(volume);
	return 0;
}
