/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "blocks.h"
#include "emile.h"

int main(int argc, char **argv)
{
	int fd;
	int fd_kernel;
	int ret;
	emile_l2_header_t header;
	off_t container_offset;
	struct emile_container *container;
	int max_blocks;
	int i;
	unsigned long kernel_image_size;

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open second stage");
		return 1;
	}

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
	{
		close(fd);
		return 2;
	}

	if (!EMILE_COMPAT(EMILE_04_SIGNATURE, read_long(&header.signature)))
	{
		fprintf(stderr, "Bad Header signature\n");
		close(fd);
		return 3;
	}

	container_offset = read_long(&header.kernel_image_offset);
	if (container_offset == 0)
	{
		fprintf(stderr, "Corrupted Header, kernel_image_offset must not be 0\n");
		close(fd);
		return 5;
	}

	printf("Container found at offset %ld\n", container_offset);

	ret = lseek(fd, container_offset, SEEK_SET);
	if (ret != container_offset) 
	{
		perror("lseek");
		close(fd);
		return 5;
	}

	container = (struct emile_container*)
					malloc(sizeof(struct emile_container));
	if (container == NULL) {
		perror("malloc");
		return 6;
	}

	ret = read(fd, container, sizeof(struct emile_container));
	if (ret != sizeof(struct emile_container))
	{
		close(fd);
		return 7;
	}

	max_blocks = container->max_blocks;
	printf("Max blocks: %d\n", max_blocks);

	free(container);
	container = (struct emile_container*)
				malloc(sizeof(struct emile_container) 
				     + max_blocks * sizeof(struct emile_block));
	if (container == NULL) {
		perror("malloc");
		return 8;
	}
	container->max_blocks = max_blocks;
	fd_kernel = open(argv[2], O_RDONLY);
	if (fd_kernel == -1)
	{
		perror("open kernel");
		return 9;
	}
	ret = scsi_create_container(fd_kernel, container);
	if (ret != 0)
		return 10;
	close(fd_kernel);

	kernel_image_size = 0;
	for(i = 0; i < max_blocks; i++)
	{
		if (container->blocks[i].count == 0)
			break;
		printf("(%d, %d) ", container->blocks[i].offset, 
				    container->blocks[i].count);
		kernel_image_size += container->blocks[i].count;
	}
	putchar('\n');
	kernel_image_size *= container->block_size;
	printf("Kernel image size: %ld bytes\n", kernel_image_size);

	ret = lseek(fd, container_offset, SEEK_SET);
	if (ret != container_offset) 
	{
		perror("lseek");
		close(fd);
		return 11;
	}

	ret = write(fd, container, sizeof(struct emile_container)
                                     + max_blocks * sizeof(struct emile_block));
	if (ret != sizeof(struct emile_container)
                                     + max_blocks * sizeof(struct emile_block))
	{
		perror("write");
		return 12;
	}

	ret = lseek(fd, 0, SEEK_SET);
	if (ret != 0) 
	{
		perror("lseek");
		close(fd);
		return 11;
	}
	header.kernel_image_size = kernel_image_size;

	ret = write(fd, &header, sizeof(header));
	if (ret != sizeof(header))
	{
		perror("write");
		close(fd);
		return 12;
	}
	

	close(fd);
	return 0;
}
