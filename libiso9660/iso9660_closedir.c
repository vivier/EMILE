/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libiso9660.h"

int iso9660_closedir(stream_DIR *dir)
{
	if (dir == NULL)
		return -1;

	free(dir);

	return 0;
}
