/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

#include "libiso9660.h"

void iso9660_close(stream_FILE *file)
{
	free(file);
}
