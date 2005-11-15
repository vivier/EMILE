/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "libiso9660.h"

extern iso9660_read_t __iso9660_device_read;

int iso9660_lseek(iso9660_FILE *file, long offset, int whence)
{
	long new_offset;

	switch(whence)
	{
	case SEEK_SET:
		new_offset = offset;
		break;
	case SEEK_CUR:
		new_offset += offset;
		break;
	case SEEK_END:
		new_offset = file->size + offset;
		break;
	default:
		return -1;
	}

	if ( (new_offset < 0) || (new_offset > file->size) )
		return -1;

	file->offset = offset;

	return offset;
}
