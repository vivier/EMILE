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

int main(int argc, char **argv)
{
	int fd;
	short id;
	int ret;

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open second stage");
		return 1;
	}

	ret = read(fd, &id, 2);
	lseek(fd, 0, SEEK_SET);
	if ( (ret == 2) && (id == 0x4C4B) )
		emile_first_set_param_scsi(fd, argv[2]);
	else
		emile_second_set_kernel_scsi(fd, argv[2]);
	

	close(fd);
	return 0;
}
