/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libblock.h"

void block_close(stream_FILE *file)
{
	if (file == NULL)
		return;

	free(file);
}
