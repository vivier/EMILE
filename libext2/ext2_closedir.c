/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include "libext2.h"
#include "ext2.h"

void ext2_closedir(stream_DIR *dir)
{
	if (dir == NULL)
		return;
	free(((ext2_DIR*)dir)->inode);
	free(dir);
}
