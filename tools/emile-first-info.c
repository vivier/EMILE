/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "libemile.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s <image>\n", argv[0]);
	fprintf(stderr, "\n     display first level boot block info\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int first_info(char* image)
{
	int fd;
	int ret;
	int drive_num;
	int second_offset;
	int second_size;

	fd = open(image, O_RDONLY);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_first_get_param(fd, &drive_num, &second_offset,
					&second_size);

	if (ret == 0)
	{
		printf("EMILE boot block identified\n\n");

		printf("Drive number: %d\n", drive_num);
		printf("Second level offset: %d\n", second_offset);
		printf("Second level size: %d\n", second_size);
	}
	else
		printf("EMILE is not installed in this bootblock\n");

	close(fd);
	return 0;
}

int main(int argc, char** argv)
{
	int ret;

	if (argc != 2)
	{
		usage(argc, argv);
		return 1;
	}

	ret = first_info(argv[1]);

	return ret;
}
