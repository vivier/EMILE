static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "libemile.h"
#include "emile.h"
#include "bootblock.h"

int emile_first_set_param(int fd, unsigned short tune_mask, int drive_num, 
			  int second_offset, int second_size)
{
	eBootBlock_t firstBlock;
	int ret;
	off_t location;

	location = lseek(fd, 0, SEEK_CUR);
	if (location == -1)
		return EEMILE_CANNOT_READ_FIRST;

	ret = read(fd, &firstBlock, sizeof(firstBlock));
	if (ret != sizeof(firstBlock))
		return EEMILE_CANNOT_READ_FIRST;

	if ( strncmp( firstBlock.boot_block_header.SysName+1,
		      "Mac Bootloader", 14) == 0 )
	{
		if (tune_mask & EMILE_FIRST_TUNE_DRIVE)
			write_short(&firstBlock.second_param_block.ioVRefNum, 
				    drive_num);

		if (tune_mask & EMILE_FIRST_TUNE_OFFSET)
			write_long(&firstBlock.second_param_block.ioPosOffset, 
				   second_offset);

		if (tune_mask & EMILE_FIRST_TUNE_SIZE)
			write_long(&firstBlock.second_param_block.ioReqCount, 
				   second_size);

		ret = lseek(fd, location, SEEK_SET);
		if (ret != 0)
			return EEMILE_CANNOT_WRITE_FIRST;

		ret = write(fd, &firstBlock, sizeof(firstBlock));
		if (ret != sizeof(firstBlock))
			return EEMILE_CANNOT_WRITE_FIRST;
	}
	else
		return EEMILE_UNKNOWN_FIRST;

	return 0;
}
