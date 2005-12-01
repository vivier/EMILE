/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#include "libcontainer.h"

int container_lseek(container_FILE *file, off_t offset, int whence)
{
	long new_offset;

	switch(whence)
	{
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset = file->offset + offset;
		break;
	default:
		return -1;
	}

	if (new_offset < 0)
		return -1;

	file->offset = new_offset;

	return new_offset;
}
