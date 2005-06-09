/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "bank.h"
#include "misc.h"
#include "glue.h"
#include "head.h"
#include "load.h"
#include "uncompress.h"

#ifdef SCSI_SUPPORT
#include "scsi.h"

static int load_container(struct emile_container* container, char* image)
{
	int target;
	int i;
	int err;

	target = container->unit_id;

	i = 0;
	while (container->blocks[i].count != 0)
	{
		err = scsi_READ(target, container->blocks[i].offset,
				container->blocks[i].count,
				image,
				container->block_size * container->blocks[i].count);
		if (err != noErr)
			return -1;

		image += container->block_size * container->blocks[i].count;
		i++;
	}

	return 0;
}
#else	/* SCSI_SUPPORT */

static int load_blocks(unsigned long offset, unsigned long size, char *image)
{
	int err;
	ParamBlockRec_t param_block;

	memset(&param_block, 0, sizeof(param_block));

	param_block.ioBuffer = (unsigned long)image;
	param_block.ioVRefNum = 1;
	param_block.ioRefNum = -5;
	param_block.ioReqCount = size;
	param_block.ioPosMode = fsFromStart;
	param_block.ioPosOffset = offset;

	err = PBReadSync(&param_block);
	if (err != noErr)
		return -1;

	return 0;
}
#endif /* SCSI_SUPPORT */

int load_image(unsigned long offset, unsigned long size, char *image)
{
	if (size == 0)
		return -1;

	if (image == NULL)
		return -1;

#ifdef SCSI_SUPPORT
	return load_container((struct emile_container*)offset, image);
#else
	return load_blocks(offset, size, image);
#endif
}

static unsigned char* gzip_image;

#ifdef SCSI_SUPPORT
unsigned char load_get_byte(unsigned long inptr)
{
	return gzip_image[inptr];
}
#else
static unsigned long buffer_size;
static unsigned long remaining_size;
static unsigned long buffer_offset;
static unsigned long disk_offset;

#define MIN(a,b)	((a) < (b) ? (a) : (b))

unsigned char load_get_byte(unsigned long inptr)
{
	if (buffer_offset == buffer_size)
	{
		unsigned to_read = MIN(buffer_size, remaining_size);

		load_image(disk_offset, to_read, gzip_image);
		buffer_offset = 0;
		remaining_size -= to_read;
		disk_offset += to_read;
	}
	return gzip_image[buffer_offset++];
}
#endif

int load_gzip(unsigned long offset, unsigned long size, char *image)
{
#ifdef SCSI_SUPPORT
	int ret;

	/* allocate memory for image */

	gzip_image = (char*)malloc(size);
	if (gzip_image == NULL)
		return -1;

	/* load image */

	ret = load_image(offset, size, gzip_image);
	if (ret == -1)
		return -1;
#else
	disk_offset = offset;
	buffer_size = size;
	remaining_size = size;
	buffer_size = 512 * 18 * 2;
	buffer_offset = buffer_size;
	gzip_image = (char*)malloc(buffer_size);
	if (gzip_image == NULL)
		return -1;
#endif

	/* uncompress */

	uncompress(image, load_get_byte);
	printf("\n");

	/* free kernel image */

	free(gzip_image);

	return 0;
}
