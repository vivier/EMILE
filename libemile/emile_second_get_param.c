/*
 *
 * (c) 2006 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "libemile.h"

int emile_second_get_param(int fd, char *kernel, char *parameters, char *initrd)
{
	int ret;
	int drive, second, size;
	char *configuration;

	/* can work on an image or directly on second level file */

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
		emile_second_get_property(configuration, "kernel", kernel);

	if (parameters != NULL)
		emile_second_get_property(configuration, "parameters", parameters);

	if (initrd != NULL)
		emile_second_get_property(configuration, "initrd", initrd);
		
	free(configuration);

	return 0;
}
