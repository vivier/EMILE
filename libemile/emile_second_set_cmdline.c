static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "libemile.h"
#include "emile.h"
#include "../second/head.h"

int emile_second_set_cmdline(int fd, char* cmdline)
{
	emile_l2_header_t header;
	off_t location;
	int ret;

	location = lseek(fd, 0, SEEK_CUR);
	if (ret == -1)
		return -1;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return -1;

	if (!EMILE_COMPAT(EMILE_02_SIGNATURE, read_long(&header.signature)))
	{
		fprintf(stderr, "Bad Header signature\n");
		return -1;
	}

	strncpy(header.command_line, cmdline, 256);
	header.command_line[255] = 0;

	ret = lseek(fd, location, SEEK_SET);
	if (ret == -1)
		return -1;

	ret = write(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return -1;

	return 0;
}
