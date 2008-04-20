/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include "libext2.h"
#include "ext2.h"

void ext2_close(stream_FILE *file)
{
	if (file == NULL)
		return;
	free(((ext2_FILE*)file)->inode);
	free(file);
}
