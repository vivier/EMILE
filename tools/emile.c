/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "libemile.h"

/* first second kernel buffer_size command_line */

int main(int argc, char **argv)
{
	int fd;
	int ret;

	/* set kernel info in second level */

	fd = open(argv[2], O_RDWR);	/* second */
	if (fd == -1)
	{
		perror("Cannot open second stage");
		return 1;
	}

	/* set buffer size */

	printf("Setting buffer size to %d\n", atoi(argv[4]));
	ret = emile_second_set_buffer_size(fd, atoi(argv[4]));

	/* set cmdline */

	lseek(fd, 0, SEEK_SET);
	printf("Setting command line to %s\n", argv[5]);
	ret = emile_second_set_cmdline(fd, argv[5]);

	/* set kernel info */

	lseek(fd, 0, SEEK_SET);
	ret = emile_second_set_kernel_scsi(fd, argv[3]);

	close(fd);

	/* set second info in first level */

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open first stage");
		return 1;
	}

	ret = emile_first_set_param_scsi(fd, argv[2]);
	close(fd);
	
	return 0;
}
