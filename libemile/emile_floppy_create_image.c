static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "libemile.h"
#include "emile.h"
#include "bootblock.h"

static int copy_file(int fd, char* file)
{
	int source;
	int size_read;
	int size_written;
	int total;
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

static int pad_image(int fd, int size)
{
	static char buffer[FLOPPY_SECTOR_SIZE];
	int size_written;
	int total;

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

static int aggregate(int fd, char* first_level, char* second_level, char* kernel_image, char* ramdisk)
{
	int ret;
	int total;

	ret = copy_file(fd, first_level);
	if (ret < 0)
		return EEMILE_CANNOT_WRITE_FIRST;
	total = ret;

	ret = copy_file(fd, second_level);
	if (ret < 0)
		return EEMILE_CANNOT_WRITE_SECOND;
	total += ret;

	if (kernel_image != NULL)
	{
		ret = copy_file(fd, kernel_image);
		if (ret < 0)
			return EEMILE_CANNOT_WRITE_KERNEL;
		total += ret;
	}

	if (ramdisk != NULL)
	{
		ret = copy_file(fd, ramdisk);
		if (ret < 0)
			return EEMILE_CANNOT_WRITE_RAMDISK;
		total += ret;
	}

	ret = pad_image(fd, 1474560 - total);
	if (ret < 0)
		return EEMILE_CANNOT_WRITE_PAD;

	return 0;
}

int emile_floppy_create_image(char* first_level, char* second_level, 
			      char* kernel_image, char* ramdisk, 
			      unsigned long buffer_size, char* image)
{
	int ret;
	int fd;

	if (image == NULL)
		return -1;

	fd = open(image, O_RDWR|O_CREAT|O_TRUNC,
			 S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd == -1)
		return EEMILE_CANNOT_CREATE_IMAGE;

	/* aggregating files: first, second, kernel, ramdisk */

	if (first_level == NULL)
	{
		close(fd);
		return EEMILE_MISSING_FIRST;
	}

	if (second_level == NULL)
	{
		close(fd);
		return EEMILE_MISSING_SECOND;
	}

	if (kernel_image == NULL)
		fprintf(stderr, "WARNING: kernel image file not defined\n");

	ret = aggregate(fd, first_level, second_level, kernel_image, ramdisk);
	if (ret != 0)
	{
		close(fd);
		return ret;
	}
	
	/* set first level info */

	lseek(fd, 0, SEEK_SET);
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

	/* set second level info */

	ret = emile_second_set_kernel(fd, kernel_image,
				      FIRST_LEVEL_SIZE + 
				      emile_file_get_size(second_level),
				      ramdisk);
	lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
	ret = emile_second_set_buffer_size(fd, buffer_size);

	close(fd);

	return ret;
}
