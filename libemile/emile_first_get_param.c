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

int emile_first_get_param(int fd, int *drive_num, int *file_ref, 
			  int *second_offset, int *second_size)
{
	eBootBlock_t firstBlock;
	int ret;

	ret = read(fd, &firstBlock, sizeof(firstBlock));
	if (ret != sizeof(firstBlock))
		return -1;

	if ( strncmp( firstBlock.boot_block_header.SysName+1,
		      "Mac Bootloader", 14) == 0 )
	{
		*drive_num = read_short(&firstBlock.second_param_block.ioVRefNum);
		*file_ref = read_short(&firstBlock.second_param_block.ioRefNum);
		*second_offset = read_long(&firstBlock.second_param_block.ioPosOffset);
		*second_size = read_long(&firstBlock.second_param_block.ioReqCount);
	}
	else
		return -1;

	return 0;
}
