/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <string.h>

#include "libemile.h"

struct emile_container *emile_second_create_mapfile(short *unit_id, char *mapfile, char* kernel)
{
	struct emile_container *container;
	int fd;
	int ret;
	short unit_id_map;
	int block_size;

	/* create container of the kernel */

	fd = open(kernel, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open kernel\n");
		return NULL;
	}

	/* get filesystem block size */

	ret = ioctl(fd, FIGETBSZ, &block_size);
	if (ret != 0) {
		close(fd);
		perror("ioctl(FIGETBSZ)");
		return NULL;
	}

	container = (struct emile_container *)malloc(block_size);
	if (container == NULL)
	{
		fprintf(stderr, "ERROR: cannot allocate memory for container\n");
		close(fd);
		return NULL;
	}
	memset(container, 0, block_size);


	ret = emile_scsi_create_container(fd, &unit_id_map, container, 
			(block_size - sizeof(struct emile_container)) / sizeof(struct emile_block));
	close(fd);

	if (ret != 0)
	{
		fprintf(stderr, "ERROR: cannot fill container\n");
		free(container);
		return NULL;
	}
	
	/* write container to map file */

	fd = open(mapfile, O_CREAT | O_WRONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot create map file (%s)\n", mapfile);
		free(container);
		return NULL;
	}

	ret = write(fd, container, block_size);

	close(fd);

	if (ret != block_size)
	{
		fprintf(stderr, "ERROR: cannot write map file (%s)\n", mapfile);
		free(container);
		return NULL;
	}

	/* now, we must know where is the map file */

	memset(container, 0, block_size);
	fd = open(mapfile, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open map file to read (%s)\n", mapfile);
		free(container);
		return NULL;
	}

	ret = emile_scsi_create_container(fd, unit_id, container, 
			(block_size - sizeof(struct emile_container)) / sizeof(struct emile_block));
	close(fd);

	if (ret != 0)
	{
		fprintf(stderr, "ERROR: cannot map map file...\n");
		free(container);
		return NULL;
	}

	if (unit_id_map != *unit_id)
	{
		fprintf(stderr, "ERROR: map file must be on the same disk as the file to map\n");
		free(container);
		return NULL;
	}

	if (container->size != block_size)
	{
		fprintf(stderr, "ERROR: map file size is bad (%d)\n", container->size);
		free(container);
		return NULL;
	}

	if (container->blocks[0].count == 0)
	{
		fprintf(stderr, "ERROR: map file is empty !\n");
		free(container);
		return NULL;
	}

	if (container->blocks[1].count != 0)
	{
		fprintf(stderr, "ERROR: map file is not contiguous\n");
		free(container);
		return NULL;
	}

	return container;
}
