/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include "bank.h"
#include "misc.h"
#include "glue.h"
#include "head.h"
#include "load.h"

#ifdef SCSI_SUPPORT
#include "scsi.h"
static char* load_container(struct emile_container* container, char* image)
{
	char* base = image;
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
		{
			free(image);
			return NULL;
		}

		image += container->block_size * container->blocks[i].count;
		i++;
	}

	return base;
}
#else	/* SCSI_SUPPORT */

static char* load_blocks(unsigned long offset, unsigned long size, char *image)
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
	{
		free(image);
		return NULL;
	}

	return image;
}
#endif /* SCSI_SUPPORT */

char* load_image(unsigned long offset, unsigned long size)
{
	char* image;

	if (size == 0)
		return NULL;

	image = malloc_contiguous(size + 4);
	if (image == 0)
	{
		free(image);
		return NULL;
	}

	image = (char*)(((unsigned long)image + 3) & 0xFFFFFFFC);

#ifdef SCSI_SUPPORT
	return load_container((struct emile_container*)offset, image);
#else
	return load_blocks(offset, size, image);
#endif
}
