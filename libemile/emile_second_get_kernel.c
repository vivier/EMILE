static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "libemile.h"
#include "emile.h"

int emile_second_get_kernel(int fd, u_int32_t *kernel_offset,
			    u_int32_t *kernel_image_size,
			    u_int32_t *kernel_size, u_int32_t *ramdisk_offset,
			    u_int32_t *ramdisk_size)
{
	emile_l2_header_t header;
	int ret;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return EEMILE_CANNOT_READ_SECOND;

	*kernel_offset = read_long(&header.kernel_image_offset);
	*kernel_image_size = read_long(&header.kernel_image_size);
	*kernel_size = read_long(&header.kernel_size);
	*ramdisk_offset = read_long(&header.ramdisk_offset);
	*ramdisk_size = read_long(&header.ramdisk_size);

	return 0;
}
