static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libemile.h"
#include "emile.h"
#include "bootblock.h"

static ssize_t copy_file(int fd, char* file)
{
	int source;
	ssize_t size_read;
	ssize_t size_written;
	ssize_t total;
	static char buffer[FLOPPY_SECTOR_SIZE];

	source = open(file, O_RDONLY);
	if (source < 0)
	{
		close(source);
		return EEMILE_CANNOT_OPEN_FILE;
	}

	total = 0;
	for(;;)
	{
		size_read = read(source, buffer, FLOPPY_SECTOR_SIZE);
		if (size_read == FLOPPY_SECTOR_SIZE)
		{
			size_written = write(fd, buffer, FLOPPY_SECTOR_SIZE);
			total += size_written;
			if (size_written != FLOPPY_SECTOR_SIZE)
			{
				close(source);
				return -1; 
			}
		}
		else
		{
			if (size_read == 0)
				break;

			memset(buffer + size_read, 0, FLOPPY_SECTOR_SIZE - size_read);
			size_written = write(fd, buffer, FLOPPY_SECTOR_SIZE);
			total += size_written;
			if (size_written != FLOPPY_SECTOR_SIZE)
			{
				close(source);
				return -1; 
			}
			break;
		}
	}

	close(source);
	return total;
}

static ssize_t pad_image(int fd, ssize_t size)
{
	static char buffer[FLOPPY_SECTOR_SIZE];
	ssize_t size_written;
	ssize_t total;

	if (size % FLOPPY_SECTOR_SIZE) {
		fprintf(stderr, 
			"WARNING: pad size is not a multiple of sector size\n");
	}

	memset(buffer, 0, FLOPPY_SECTOR_SIZE);
	total = 0;
	while (size > 0) {
		size_written = write(fd, buffer, FLOPPY_SECTOR_SIZE);
		total += size_written;
		if (size_written != FLOPPY_SECTOR_SIZE) {
			return total; 
		}
		size -= size_written;
	}
	return total;
}

int emile_is_url(char *path)
{
	return path && ((strncmp(path, "iso9660:", strlen("iso9660:")) == 0) ||
	       (strncmp(path, "container:", strlen("container:")) == 0) ||
	       (strncmp(path, "block:", strlen("block:")) == 0) ||
	       (strncmp(path, "ext2:", strlen("ext2")) == 0));
}

int emile_floppy_create(char *image, char* first_level, char* second_level)
{
	int fd;
	off_t offset;
	ssize_t size;
	int ret;

	if (first_level == NULL)
	{
		return EEMILE_MISSING_FIRST;
	}

	if (second_level == NULL)
	{
		return EEMILE_MISSING_SECOND;
	}

	fd = open(image, O_RDWR|O_CREAT|O_TRUNC,
			 S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd == -1)
		return EEMILE_CANNOT_CREATE_IMAGE;

	size = copy_file(fd, first_level);
	if (size < 0)
		return EEMILE_CANNOT_WRITE_FIRST;

	size = copy_file(fd, second_level);
	if (size < 0)
		return EEMILE_CANNOT_WRITE_SECOND;

	/* set first level info */

	offset = lseek(fd, 0, SEEK_SET);
	ret = emile_first_set_param(fd, EMILE_FIRST_TUNE_DRIVE |
					EMILE_FIRST_TUNE_OFFSET|
					EMILE_FIRST_TUNE_SIZE, 
					1, FIRST_LEVEL_SIZE, 
					emile_file_get_size(second_level));
	if (ret != 0)
	{
		close(fd);
		return ret;
	}

	lseek(fd, offset, SEEK_SET);

	return fd;
}

char* emile_floppy_add(int fd, char *image)
{
	off_t offset;
	ssize_t size;
	char buf[64];
	
	lseek(fd, 0, SEEK_END);

	offset = lseek(fd, 0, SEEK_CUR);
	if (offset == (off_t)-1)
		return NULL;

	size = copy_file(fd, image);
	if (size == -1)
		return NULL;

	sprintf(buf, "block:(fd0)0x%jx,0x%zx", offset / 512, size);

	return strdup(buf);
}

int emile_floppy_close(int fd)
{
	int ret;
	off_t offset;

	lseek(fd, 0, SEEK_END);

	offset = lseek(fd, 0, SEEK_CUR);

	ret = pad_image(fd, 1474560 - offset);
	if (ret < 0)
		return EEMILE_CANNOT_WRITE_PAD;
	
	close(fd);

	return 0;
}

int emile_floppy_create_image(char* first_level, char* second_level, 
			      char* kernel_image, char* ramdisk, 
			      char* image)
{
	int ret;
	int fd;
	char *kernel_url = NULL;
	char *ramdisk_url = NULL;

	if (image == NULL)
		return -1;

	if (kernel_image == NULL) {
		fprintf(stderr, "ERROR: kernel image file not defined\n");
		return -1;
	}

	fd = emile_floppy_create(image, first_level, second_level);
	if (fd == -1)
		return -1;

	if ( emile_is_url(kernel_image) )
		kernel_url = strdup(kernel_image);
	else
		kernel_url = emile_floppy_add(fd, kernel_image);

	if ( ramdisk && emile_is_url(ramdisk) )
		ramdisk_url = strdup(ramdisk);
	else if (ramdisk)
		ramdisk_url = emile_floppy_add(fd, ramdisk);

	/* set second level info */

	ret = emile_second_set_param(fd, kernel_url, NULL, ramdisk_url);

	emile_floppy_close(fd);

	free(kernel_url);
	free(ramdisk_url);

	return ret;
}
