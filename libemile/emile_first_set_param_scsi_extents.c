static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004-2006 Laurent Vivier <Laurent@Vivier.EU>
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

int emile_first_set_param_scsi_extents( int fd, int drive_num, int second_offset, int size, int blocksize)
{
	int ret;
	char first[1024];
	int current;
	/* unsigned short max_blocks; */
	int location;

	location = lseek(fd, 0, SEEK_CUR);
	if (location == -1)
		return EEMILE_CANNOT_READ_FIRST;

	ret = read(fd, first, 1024);
	if (ret == -1)
		return EEMILE_CANNOT_READ_FIRST;

	/* max_blocks = read_short((u_int16_t*)&first[1022]) / 6; */

	write_short((u_int16_t*)&first[1014], blocksize);
	write_short((u_int16_t*)&first[1016], drive_num);

	write_long((u_int32_t*)&first[1018], 0);
	current = 1014;

	current -= 2;
	write_short((u_int16_t*)&first[current], (size + blocksize - 1) / blocksize);
	current -= 4;
	write_long((u_int32_t*)&first[current], second_offset);

	/* mark end of blocks list */
	current -= 2;
	write_short((u_int16_t*)(&first[current]), 0);
	/* set second level size */
	write_long((u_int32_t*)&first[1018], (size + blocksize - 1) / blocksize * blocksize);
	
	ret = lseek(fd, location, SEEK_SET);
	if (ret != location)
		return EEMILE_CANNOT_WRITE_FIRST;

	ret = write(fd, first, 1024);
	if (ret == -1)
		return EEMILE_CANNOT_WRITE_FIRST;

	return 0;
}
