/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "libemile.h"
#include "emile.h"
#include "bootblock.h"

int emile_second_set_configuration(int fd, char *configuration)
{
	emile_l2_header_t header;
	int ret;
	int size;
	int len;

	if (configuration == NULL)
		return EEMILE_CANNOT_READ_SECOND;

	len = strlen (configuration) + 1;	/* + 1 for ending 0 */

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return EEMILE_CANNOT_READ_SECOND;

	if (!EMILE_COMPAT(EMILE_06_SIGNATURE, read_long(&header.signature)))
		return EEMILE_INVALID_SECOND;

	size = read_short(&header.conf_size);
	if (len > size)
		return EEMILE_INVALID_SECOND;

	ret = write(fd, configuration, len);
	if (ret != len)
		return EEMILE_CANNOT_WRITE_SECOND;

	return 0;
}
