#include <string.h>

#include "libfloppy.h"

size_t floppy_read(floppy_FILE *file, void *ptr, size_t size)
{
	int read = 0;
	int ret;

	while (size != 0)
	{
		int part;
		int cylinder_nb = file->offset / CYLINDER_SIZE;
		int cylinder_offset = file->offset % CYLINDER_SIZE;

		if (cylinder_nb != file->current_cylinder)
		{
			ret = floppy_read_sector((cylinder_nb * CYLINDER_SIZE) >> SECTOR_SIZE_BITS,
						 file->cylinder, 
						 CYLINDER_SIZE);
			if (ret == -1)
				return read;
			file->current_cylinder = cylinder_nb;
		}

		part = CYLINDER_SIZE - cylinder_offset;
		if (part > size)
			part = size;
		memcpy(ptr, file->cylinder + cylinder_offset, part);

		size -= part;
		ptr = (char*)ptr + part;
		file->offset += part;
		read += part;
	}

	return read;
}
