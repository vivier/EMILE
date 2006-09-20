/*
 *
 * (c) 2006 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "libemile.h"

int emile_second_set_param(int fd, char *kernel, char *parameters, char *initrd)
{
	int ret;
	char *configuration;
	off_t offset;

	offset = lseek(fd, 0, SEEK_CUR);

	configuration = emile_second_get_configuration(fd);
	if (configuration == NULL)
		return -1;

	if (kernel != NULL)
		emile_second_set_property(configuration, "kernel", kernel);

	if (parameters != NULL)
		emile_second_set_property(configuration, "parameters", parameters);

	if (initrd != NULL)
		emile_second_set_property(configuration, "initrd", initrd);
		
	emile_second_set_property(configuration, "vga", "default");

	ret = lseek(fd, offset, SEEK_SET);
	if (ret == -1)
		return -1;

	ret = emile_second_set_configuration(fd, configuration);
	if (ret != 0)
		return -1;

	free(configuration);

	return 0;
}
