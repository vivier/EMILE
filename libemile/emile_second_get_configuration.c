/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@Vivier.EU>
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
	int8_t *conf;
	emile_l2_header_t header;
	int ret;
	int size;

	memset(&header, 0, sizeof(header));

	ret = read(fd, &header, sizeof(header));
	if (!EMILE_COMPAT(EMILE_06_SIGNATURE, read_long(&header.signature)))
	{
		off_t offset;
		offset = lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
		ret = read(fd, &header, sizeof(header));
		if (!EMILE_COMPAT(EMILE_06_SIGNATURE, read_long(&header.signature)))
		{
			lseek(fd, offset, SEEK_SET);
			return NULL;
		}
	}

	if (ret != sizeof(header))
		return NULL;

	size = read_short(&header.conf_size);
	conf = (int8_t*)malloc(size);
	if (conf == NULL)
		return NULL;

	ret = read(fd, conf, size);
	if (ret != size)
	{
		free(conf);
		return NULL;
	}
	
	return conf;
}
