/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdlib.h>

#include "libiso9660.h"

void iso9660_dir_close(iso9660_DIR *dir)
{
	free(dir);
}
