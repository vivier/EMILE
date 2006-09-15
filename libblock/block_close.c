/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libblock.h"

int block_close(block_FILE *file)
{
	if (file == NULL)
		return -1;

	free(file);

	return 0;
}
