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

int emile_second_set_output(int fd, unsigned int enable_mask, 
			    unsigned int disable_mask, 
			    unsigned int bitrate0, int datasize0,
			    int parity0, int stopbits0,
			    unsigned int bitrate1, int datasize1,
			    int parity1, int stopbits1, int gestaltid)
{
	emile_l2_header_t header;
	off_t location;
	int ret;

	location = lseek(fd, 0, SEEK_CUR);
	if (location == -1)
		return location;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return -1;

	if (!EMILE_COMPAT(EMILE_03_SIGNATURE, read_long(&header.signature)))
	{
		fprintf(stderr, "Bad Header signature\n");
		return -1;
	}
	
	header.console_mask |= enable_mask;
	header.console_mask &= ~disable_mask;

	if (bitrate0)
		header.serial0_bitrate = bitrate0;
	if (bitrate1)
		header.serial1_bitrate = bitrate1;

	if (datasize0 != -1)
		header.serial0_datasize = datasize0;
	if (datasize1 != -1)
		header.serial1_datasize = datasize1;

	if (stopbits0 != -1)
		header.serial0_stopbits = stopbits0;
	if (stopbits1 != -1)
		header.serial1_stopbits = stopbits1;

	if (parity0 != -1)
		header.serial0_parity = parity0;
	if (parity1 != -1)
		header.serial1_parity = parity1;

	header.gestaltID = gestaltid;	/* 0 means unset ... */

	ret = lseek(fd, location, SEEK_SET);
	if (ret == -1)
	{
		perror("Cannot go to buffer offset");
		close(fd);
		return 8;
	}

	ret = write(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return -1;

	return 0;
}
