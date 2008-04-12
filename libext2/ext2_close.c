/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include "libext2.h"

void ext2_close(ext2_FILE *file)
{
	if (file == NULL)
		return;
	free(file->inode);
	free(file);
}
