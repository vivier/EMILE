static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "libemile.h"
#include "emile.h"

int emile_second_set_kernel_scsi(int fd, char *kernel_name)
{
	int fd_kernel;
	int ret;
	emile_l2_header_t header;
	off_t container_offset;
	struct emile_container *container;
	int max_blocks;
	int i;
	unsigned long kernel_image_size;

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return -1;

	if (!EMILE_COMPAT(EMILE_04_SIGNATURE, read_long(&header.signature)))
		return -1;

	container_offset = read_long(&header.kernel_image_offset);
	if (container_offset == 0)
		return -1;

	ret = lseek(fd, container_offset, SEEK_SET);
	if (ret != container_offset) 
		return -1;

	container = (struct emile_container*)
					malloc(sizeof(struct emile_container));
	if (container == NULL)
		return -1;

	ret = read(fd, container, sizeof(struct emile_container));
	if (ret != sizeof(struct emile_container))
		return -1;

	max_blocks = container->max_blocks;

	free(container);
	container = (struct emile_container*)
				malloc(sizeof(struct emile_container) 
				     + max_blocks * sizeof(struct emile_block));
	if (container == NULL)
		return -1;

	container->max_blocks = max_blocks;
	fd_kernel = open(kernel_name, O_RDONLY);
	if (fd_kernel == -1)
		return -1;

	ret = emile_scsi_create_container(fd_kernel, container);
	if (ret != 0)
		return 10;
	close(fd_kernel);

	kernel_image_size = 0;
	for(i = 0; i < max_blocks; i++)
	{
		if (container->blocks[i].count == 0)
			break;
		kernel_image_size += container->blocks[i].count;
	}
	kernel_image_size *= container->block_size;

	ret = lseek(fd, container_offset, SEEK_SET);
	if (ret != container_offset) 
		return -1;

	ret = write(fd, container, sizeof(struct emile_container)
                                     + max_blocks * sizeof(struct emile_block));
	if (ret != sizeof(struct emile_container)
                                     + max_blocks * sizeof(struct emile_block))
		return -1;

	ret = lseek(fd, 0, SEEK_SET);
	if (ret != 0) 
		return -1;

	header.kernel_image_size = kernel_image_size;

	ret = write(fd, &header, sizeof(header));
	if (ret != sizeof(header))
		return -2;

	return 0;
}
