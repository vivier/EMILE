static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004-2006 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libemile.h"
#include "emile.h"

#define BLOCK_SIZE	512	/* FIXME: should ask the disk driver */


int emile_first_set_param_scsi_extents( int fd, int drive_num, int second_offset, int size)
{
	int fd;
	int ret;
	char first[1024];
	int current;
	unsigned short max_blocks;
	int location;

	location = lseek(fd, 0, SEEK_CUR);
	if (location == -1)
		return EEMILE_CANNOT_READ_FIRST;

	ret = read(fd, first, 1024);
	if (ret == -1)
		return EEMILE_CANNOT_READ_FIRST;

	max_blocks = read_short((u_int16_t*)&first[1022]) / 6;

	write_short((u_int16_t*)&first[1014], BLOCK_SIZE);
	write_short((u_int16_t*)&first[1016], drive_num);

	write_long((u_int32_t*)&first[1018], 0);
	current = 1014;

	current -= 2;
	write_short((u_int16_t*)&first[current], (size + BLOCK_SIZE - 1) / BLOCK_SIZE);
	current -= 4;
	write_long((u_int32_t*)&first[current], second_offset);

	/* mark end of blocks list */
	current -= 2;
	write_short((u_int16_t*)(&first[current]), 0);
	/* set second level size */
	write_long((u_int32_t*)&first[1018], (size + BLOCK_SIZE - 1) / BLOCK_SIZE * BLOCK_SIZE);
	
	ret = lseek(fd, location, SEEK_SET);
	if (ret != 0)
		return EEMILE_CANNOT_WRITE_FIRST;

	ret = write(fd, first, 1024);
	if (ret == -1)
		return EEMILE_CANNOT_WRITE_FIRST;

	return 0;
}
