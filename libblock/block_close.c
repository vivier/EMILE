/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
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
