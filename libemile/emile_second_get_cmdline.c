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

int emile_second_get_cmdline(int fd, char* cmdline)
{
	emile_l2_header_t header;
	int ret;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return EEMILE_CANNOT_READ_SECOND;

	if (!EMILE_COMPAT(EMILE_02_SIGNATURE, read_long(&header.signature)))
	{
		fprintf(stderr, "Bad Header signature\n");
		return EEMILE_INVALID_SECOND;
	}

	strncpy(cmdline, header.command_line, 256);

	return 0;
}
