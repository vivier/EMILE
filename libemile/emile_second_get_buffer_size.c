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

int emile_second_get_buffer_size(int fd, u_int32_t *buffer_size)
{
	emile_l2_header_t header;
	int ret;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return EEMILE_CANNOT_READ_SECOND;

	*buffer_size = read_long(&header.kernel_size);

	return 0;
}
