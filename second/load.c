/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include "misc.h"
#include "glue.h"
#include "load.h"

char* load_image(unsigned long offset, unsigned long size)
{
	int err;
	char* image;
	ParamBlockRec_t param_block;

	if (size == 0)
		return NULL;

	image = malloc(size + 4);
	if (image == 0)
	{
		free(image);
		return NULL;
	}

	image = (char*)(((unsigned long)image + 3) & 0xFFFFFFFC);

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
