/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "libemile.h"
#include "emile.h"

char* emile_second_get_configuration(int fd)
{
	char *conf;
	emile_l2_header_t header;
	int ret;
	int size;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return NULL;

	size = read_short(&header.conf_size);
	conf = (char*)malloc(size);
	if (conf == NULL)
		return NULL;

	ret = read(fd, conf, size);
	if (ret != size)
		return NULL;
	
	return conf;
}
