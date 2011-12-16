/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
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

int emile_second_set_configuration(int fd, int8_t *configuration)
{
	emile_l2_header_t header;
	int ret;
	int size;
	int len;
	char *buf;

	if (configuration == NULL)
		return EEMILE_CANNOT_READ_SECOND;

	len = strlen ((char*)configuration) + 1;	/* + 1 for ending 0 */

	memset(&header, 0, sizeof(header));

	ret = read(fd, &header, sizeof(header));
	if (!EMILE_COMPAT(EMILE_06_SIGNATURE, read_long(&header.signature)))
	{
		lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
		ret = read(fd, &header, sizeof(header));
		if (!EMILE_COMPAT(EMILE_06_SIGNATURE, read_long(&header.signature)))
			return EEMILE_INVALID_SECOND;
	}

	if (ret != sizeof(header))
		return EEMILE_CANNOT_READ_SECOND;

	size = read_short(&header.conf_size);
	if (len > size)
		return EEMILE_INVALID_SECOND;

	buf = malloc(size);
	memset(buf, 0, size);
	memcpy(buf, configuration, len);

	ret = write(fd, buf, size);
	if (ret != size)
		return EEMILE_CANNOT_WRITE_SECOND;
	free(buf);

	return 0;
}
