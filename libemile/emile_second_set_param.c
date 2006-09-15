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
	int drive, second, size;
	char *configuration;
	off_t offset;

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
