/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libemile.h"

#define MAPFILE_SIZE	4096

struct emile_container *emile_second_create_mapfile(char *mapfile, char* kernel)
{
	struct emile_container *container;
	int fd;
	int ret;

	container = (struct emile_container *)malloc(MAPFILE_SIZE);
	if (container == NULL)
	{
		fprintf(stderr, "ERROR: cannot allocate memory for container\n");
		return NULL;
	}

	/* create container of the kernel */

	fd = open(kernel, O_RDONLY);
	if (fd == -1)
	{
		free(container);
		fprintf(stderr, "ERROR: cannot open kernel\n");
		return NULL;
	}

	ret = emile_scsi_create_container(fd, container, 
			(MAPFILE_SIZE - sizeof(struct emile_container)) / sizeof(struct emile_block));
	close(fd);

	if (ret != 0)
	{
		fprintf(stderr, "ERROR: cannot fill container\n");
		free(container);
		return NULL;
	}
	
	/* write container to map file */

	fd = open(mapfile, O_CREAT | O_EXCL | O_WRONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot create map file (%s)\n", mapfile);
		free(container);
		return NULL;
	}

	ret = write(fd, container, MAPFILE_SIZE);

	close(fd);

	if (ret != MAPFILE_SIZE)
	{
		fprintf(stderr, "ERROR: cannot write map file (%s)\n", mapfile);
		free(container);
		return NULL;
	}

	/* now, we must know where is the map file */

	fd = open(mapfile, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open map file to read (%s)\n", mapfile);
		free(container);
		return NULL;
	}

	ret = emile_scsi_create_container(fd, container, 
			(MAPFILE_SIZE - sizeof(struct emile_container)) / sizeof(struct emile_block));
	close(fd);

	if (ret != 0)
	{
		fprintf(stderr, "ERROR: cannot map map file...\n");
		free(container);
		return NULL;
	}

	if (container->size != MAPFILE_SIZE)
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
