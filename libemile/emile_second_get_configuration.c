/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "libemile.h"
#include "emile.h"

int8_t* emile_second_get_configuration(int fd)
{
	int8_t *conf = NULL;
	emile_l2_header_t header;
	int ret;
	int size;
	off_t offset;

	memset(&header, 0, sizeof(header));

	ret = read(fd, &header, sizeof(header));
	if (!EMILE_COMPAT(EMILE_06_SIGNATURE, read_long(&header.signature)))
	{
		offset = lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
		ret = read(fd, &header, sizeof(header));
		if (!EMILE_COMPAT(EMILE_06_SIGNATURE, read_long(&header.signature)))
			goto exit;
	}

	if (ret != sizeof(header))
		goto exit;

	size = read_short(&header.conf_size);
	conf = (int8_t*)malloc(size);
	if (conf == NULL)
		goto exit;

	ret = read(fd, conf, size);
	if (ret != size)
		return NULL;
	
exit:
	lseek(fd, offset, SEEK_SET);
	return conf;
}
