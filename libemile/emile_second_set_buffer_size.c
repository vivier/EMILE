static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libemile.h"
#include "emile.h"
#include "bootblock.h"

int emile_second_set_buffer_size(int fd, u_int32_t buffer_size)
{
	emile_l2_header_t header;
	int ret;
	off_t location;

	location = lseek(fd, 0, SEEK_CUR);
	if (ret == -1)
		return EEMILE_CANNOT_READ_SECOND;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return EEMILE_CANNOT_READ_SECOND;

	write_long(&header.kernel_size, buffer_size);

	ret = lseek(fd, location, SEEK_SET);
	if (ret == -1)
		return EEMILE_CANNOT_WRITE_SECOND;

	ret = write(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return EEMILE_CANNOT_WRITE_SECOND;

	return 0;
}
