static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "libemile.h"
#include "emile.h"
#include "../second/head.h"

int emile_second_get_output(int fd, u_int32_t *console_mask,
			    u_int32_t *bitrate0, int *datasize0,
			    int *parity0, int *stopbits0,
			    u_int32_t *bitrate1, int *datasize1,
			    int *parity1, int *stopbits1,
			    int *gestaltid)
{
	emile_l2_header_t header;
	int ret;

	ret = read(fd, &header, sizeof(header));

	if (ret != sizeof(header))
		return -1;

	if (!EMILE_COMPAT(EMILE_03_SIGNATURE, read_long(&header.signature)))
	{
		fprintf(stderr, "Bad Header signature\n");
		return -1;
	}

	*console_mask = read_long(&header.console_mask);
	*bitrate0 = read_long(&header.serial0_bitrate);
	*datasize0 = header.serial0_datasize;
	*parity0 = header.serial0_parity;
	*stopbits0 = header.serial0_stopbits;
	*bitrate1 = read_long(&header.serial1_bitrate);
	*datasize1 = header.serial1_datasize;
	*parity1 = header.serial1_parity;
	*stopbits1 = header.serial1_stopbits;

	*gestaltid = read_long(&header.gestaltID);

	return 0;
}
