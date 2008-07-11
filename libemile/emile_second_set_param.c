/*
 *
 * (c) 2006 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "libemile.h"
#include "libconfig.h"

int emile_second_set_param(int fd, char *kernel, char *parameters, char *initrd)
{
	int ret;
	int8_t *configuration;
	off_t offset;

	offset = lseek(fd, 0, SEEK_CUR);

	configuration = emile_second_get_configuration(fd);
	if (configuration == NULL)
		return -1;

	if (kernel != NULL)
	{
		if (*kernel)
			config_set_property(configuration, "kernel", kernel);
		else
			config_remove_property(configuration, "kernel");
	}

	if (parameters != NULL)
	{
		if (*parameters)
			config_set_property(configuration, "args", parameters);
		else
			config_remove_property(configuration, "args");
	}

	if (initrd != NULL)
	{
		if (*initrd)
			config_set_property(configuration, "initrd", initrd);
		else
			config_remove_property(configuration, "initrd");
	}
		
	config_set_property(configuration, "vga", "default");

	ret = lseek(fd, offset, SEEK_SET);
	if (ret == -1)
		return -1;

	ret = emile_second_set_configuration(fd, configuration);
	if (ret != 0)
		return -1;

	free(configuration);

	return 0;
}
