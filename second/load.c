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
