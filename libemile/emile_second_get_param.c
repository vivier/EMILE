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

int emile_second_get_param(int fd, char *kernel, char *parameters, char *initrd)
{
	int ret;
	int drive, second, size;
	int8_t *configuration;

	/* can work on an image or directly on second level file */

	lseek(fd, 0, SEEK_SET);
	ret = emile_first_get_param(fd, &drive, &second, &size);
	if (ret == EEMILE_UNKNOWN_FIRST)
	{
		/* should be a second level file */

		ret = lseek(fd, 0, SEEK_SET);
		if (ret == -1)
			return -1;
	}

	configuration = emile_second_get_configuration(fd);
	if (configuration == NULL)
		return -1;

	if (kernel != NULL)
		config_get_property(configuration, "kernel", kernel);

	if (parameters != NULL)
		config_get_property(configuration, "parameters", parameters);

	if (initrd != NULL)
		config_get_property(configuration, "initrd", initrd);
		
	free(configuration);

	return 0;
}
