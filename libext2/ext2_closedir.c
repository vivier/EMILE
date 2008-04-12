/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include "libext2.h"

void ext2_closedir(ext2_DIR *dir)
{
	if (dir == NULL)
		return;
	free(dir->inode);
	free(dir);
}
