static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libemile.h"

#define BLOCK_SIZE	512	/* FIXME: should ask the disk driver */

int emile_first_set_param_scsi(int fd, char *second_name)
{
	int ret;
	int fd_second;
	char first[1024];
	int i;
	int current;
	struct emile_container *container;
	unsigned short max_blocks;
	unsigned short *first_max_blocks = (unsigned short*)&first[1022];
	unsigned long *second_size = (unsigned long*)&first[1018];
	unsigned short *unit_id = (unsigned short*)&first[1016];
	unsigned short *block_size = (unsigned short*)&first[1014];
	unsigned short *count;
	unsigned long *offset;
	off_t location;

	location = lseek(fd, 0, SEEK_CUR);
	if (location == -1)
		return EEMILE_CANNOT_READ_FIRST;

	ret = read(fd, first, 1024);
	if (ret == -1)
		return EEMILE_CANNOT_READ_FIRST;

	max_blocks = *first_max_blocks / 6;

	container = (struct emile_container*)
				malloc(sizeof(struct emile_container) 
				     + max_blocks * sizeof(struct emile_block));
	if (container == NULL)
		return EEMILE_MALLOC_ERROR;

	fd_second = open(second_name, O_RDONLY);
	if (fd_second == -1)
		return EEMILE_CANNOT_OPEN_FILE;

	ret = emile_scsi_create_container(fd_second, unit_id, container, max_blocks);
	if (ret != 0)
		return ret;
	close(fd_second);

	*block_size = BLOCK_SIZE;

	*second_size = 0;
	current = 1014;
	for(i = 0; i < max_blocks - 1; i++)
	{
		current -= 2;
		count = (short*)(&first[current]);
		*count = container->blocks[i].count;
		if (container->blocks[i].count == 0)
			break;
		current -= 4;
		offset = (long*)(&first[current]);
		*offset = container->blocks[i].offset;
		(*second_size) += container->blocks[i].count;
	}
	putchar('\n');
	/* mark end of blocks list */
	current -= 2;
	count = (short*)(&first[current]);
	*count = 0;
	/* set second level size */
	(*second_size) *= BLOCK_SIZE;

	ret = lseek(fd, location, SEEK_SET);
	if (ret != 0) 
		return EEMILE_CANNOT_WRITE_FIRST;

	ret = write(fd, first, 1024);
	if (ret == -1)
		return EEMILE_CANNOT_WRITE_FIRST;
	
	return 0;
}
