/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "emile.h"
#include "emile-first.h"
#include "emile-second.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s -f <first level> -s <second level> -i <kernel image> -r <ramdisk> -b <buffer size> <image>\n", argv[0]);
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static unsigned long get_size(char* file)
{
	struct stat result;

	stat(file, &result);

	return (result.st_size + SECTOR_SIZE - 1) / SECTOR_SIZE * SECTOR_SIZE;
}

static int second_tune(int fd, char* second_level, char *kernel_image, unsigned buffer_size, char* ramdisk)
{
	emile_l2_header_t header;
	int ret;

	ret = lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
	if (ret == -1)
	{
		close(fd);
		return 3;
	}

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
	{
		close(fd);
		return 7;
	}

	write_long(&header.kernel_image_offset, 
			FIRST_LEVEL_SIZE + get_size(second_level));
	write_long(&header.kernel_image_size, 
			get_size(kernel_image));
	write_long(&header.kernel_size, buffer_size);

	if (ramdisk == NULL)
	{
		write_long(&header.ramdisk_offset, 0);
		write_long(&header.ramdisk_size, 0);
	}
	else
	{
		write_long(&header.ramdisk_offset, 
			   read_long(&header.kernel_image_offset) + 
			   read_long(&header.kernel_image_size));
		write_long(&header.ramdisk_size, get_size(ramdisk));
	}
	printf("Setting second level info: \n");
	printf("kernel offset %d, kernel size %d, buffer size %d\n",
		read_long(&header.kernel_image_offset),
		read_long(&header.kernel_image_size),
		read_long(&header.kernel_size));
	printf("ramdisk offset %d, ramdisk size %d\n",
		read_long(&header.ramdisk_offset),
		read_long(&header.ramdisk_size));

	ret = lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
	if (ret == -1)
	{
		close(fd);
		return 3;
	}

	ret = write(fd, &header, sizeof(header));
	if (ret != sizeof(header))
	{
		close(fd);
		return 7;
	}

	return 0;
}

static int first_tune(int fd, char* second_level)
{
	int ret;
	eBootBlock_t firstBlock;

	ret = lseek(fd, 0, SEEK_SET);
	if (ret == -1)
		return 9;

	ret = read(fd, &firstBlock, sizeof(firstBlock));
        if (ret != sizeof(firstBlock))
                return 10;

	/* drive number */

	write_short(&firstBlock.second_param_block.ioVRefNum, 1);

	/* second offset */

	write_long(&firstBlock.second_param_block.ioPosOffset, FIRST_LEVEL_SIZE);

	/* second size */

	write_long(&firstBlock.second_param_block.ioReqCount, get_size(second_level));

	printf(
	"Setting second level position to: drive %d, offset %d, size %d\n", 
	read_short(&firstBlock.second_param_block.ioVRefNum), 
	read_long(&firstBlock.second_param_block.ioPosOffset), 
	read_long(&firstBlock.second_param_block.ioReqCount));

	/* writing */

	ret = lseek(fd, 0, SEEK_SET);
	if (ret == -1)
		return 11;

	ret = write(fd, &firstBlock, sizeof(firstBlock));
	if (ret != sizeof(firstBlock))
		return 12;

	return 0;
}

static int copy_file(int fd, char* file)
{
	int source;
	int size_read;
	int size_written;
	int total;
	static char buffer[SECTOR_SIZE];

	if (fd < 0)
		return -1;

	source = open(file, O_RDONLY);
	if (source < 0)
	{
		close(source);
		return -1;
	}

	printf("Copying %s: ", file);
	total = 0;
	for(;;)
	{
		size_read = read(source, buffer, SECTOR_SIZE);
		if (size_read == SECTOR_SIZE)
		{
			size_written = write(fd, buffer, SECTOR_SIZE);
			total += size_written;
			if (size_written != SECTOR_SIZE)
			{
				close(source);
				return -1; 
			}
		}
		else
		{
			if (size_read == 0)
				break;

			memset(buffer + size_read, 0, SECTOR_SIZE - size_read);
			size_written = write(fd, buffer, SECTOR_SIZE);
			total += size_written;
			if (size_written != SECTOR_SIZE)
			{
				close(source);
				return -1; 
			}
			break;
		}
	}

	printf("%d bytes\n", total);
	close(source);
	return total;
}

static int pad_image(int fd, int size)
{
	static char buffer[SECTOR_SIZE];
	int size_written;
	int total;

	if (fd < 0)
		return -1;

	if (size % SECTOR_SIZE) {
		fprintf(stderr, 
			"WARNING: pad size is not a multiple of sector size\n");
	}

	memset(buffer, 0, SECTOR_SIZE);
	total = 0;
	while (size > 0) {
		size_written = write(fd, buffer, SECTOR_SIZE);
		total += size_written;
		if (size_written != SECTOR_SIZE) {
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
		return 6;
	total = ret;

	ret = copy_file(fd, second_level);
	if (ret < 0)
		return 6;
	total += ret;

	ret = copy_file(fd, kernel_image);
	if (ret < 0)
		return 6;
	total += ret;

	if (ramdisk != NULL)
	{
		ret = copy_file(fd, ramdisk);
		if (ret < 0)
			return 6;
		total += ret;
	}

	ret = pad_image(fd, 1474560 - total);
	if (ret < 0)
		return 6;

	return 0;
}

int create_image(char* first_level, char* second_level, char* kernel_image, 
		 char* ramdisk, unsigned long buffer_size, char* image)
{
	int ret;
	int fd;

	if (image == NULL)
	{
		fprintf(stderr, "Image file not defined\n");
		return 2;
	}

	printf("Creating %s\n", image);
	fd = open(image, O_RDWR|O_CREAT|O_TRUNC,
			 S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	/* aggregating files: first, second, kernel, ramdisk */

	if (first_level == NULL)
	{
		fprintf(stderr, "first level file not defined\n");
		close(fd);
		return 3;
	}

	if (second_level == NULL)
	{
		fprintf(stderr, "second level file not defined\n");
		close(fd);
		return 4;
	}

	if (kernel_image == NULL)
	{
		fprintf(stderr, "kernel image file not defined\n");
		close(fd);
		return 5;
	}

	ret = aggregate(fd, first_level, second_level, kernel_image, ramdisk);
	if (ret != 0)
	{
		perror("Cannot create image\n");
		close(fd);
		return 6;
	}
	
	/* set first level info */

	ret = first_tune(fd, second_level);
	if (ret != 0)
	{
		close(fd);
		return 7;
	}

	/* set second level info */

	ret = second_tune(fd, second_level, kernel_image, buffer_size, ramdisk);
	if (ret != 0)
	{
		close(fd);
		return 8;
	}

	close(fd);

	return 0;
}

int main(int argc, char** argv)
{
	char* first_level = NULL;
	char* second_level = NULL;
	char* kernel_image = NULL;
	char* ramdisk = NULL;
	unsigned long buffer_size = 0;
	char* image = NULL;
	int i;
	int ret;

	ASSERT_BBH(
	{fprintf(stderr,"Internal Error: Bad BootBlkHdr size\n"); exit(1);});
	ASSERT_PBR(
	{fprintf(stderr,"Internal Error: Bad ParamBlockRec size\n"); exit(1);});
	ASSERT_BB(
	{fprintf(stderr,"Internal Error: Bad boot block size\n"); exit(1);});

	if ((argc != 12) && (argc != 10))
	{
		usage(argc, argv);
		return 1;
	}

	for (i = 1; i < argc; i++)
	{
		if (strcmp("-f", argv[i]) == 0)
		{
			if (first_level != NULL)
			{
				fprintf(stderr, "Duplicate definition of -f\n");
				return 1;
			}
			i++;
			if (i < argc)
				first_level = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -f\n");
				return 1;
			}
		}
		else if (strcmp("-s", argv[i]) == 0)
		{
			if (second_level != NULL)
			{
				fprintf(stderr, "Duplicate definition of -s\n");
				return 1;
			}
			i++;
			if (i < argc)
				second_level = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -s\n");
				return 1;
			}
		}
		else if (strcmp("-i", argv[i]) == 0)
		{
			if (kernel_image != NULL)
			{
				fprintf(stderr, "Duplicate definition of -i\n");
				return 1;
			}
			i++;
			if (i < argc)
				kernel_image = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -i\n");
				return 1;
			}
		}
		else if (strcmp("-b", argv[i]) == 0)
		{
			if (buffer_size != 0)
			{
				fprintf(stderr, "Duplicate definition of -b\n");
				return 1;
			}
			i++;
			if (i < argc)
				buffer_size = atol(argv[i]);
			else
			{
				fprintf(stderr, "Missing argument for -b\n");
				return 1;
			}
		}
		else if (strcmp("-r", argv[i]) == 0)
		{
			if (ramdisk != 0)
			{
				fprintf(stderr, "Duplicate definition of -r\n");
				return 1;
			}
			i++;
			if (i < argc)
				ramdisk = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -r\n");
				return 1;
			}
		}
		else
		{
			if (image != NULL)
			{
				fprintf(stderr, "Unknown parameter %s\n", 
						argv[i]);
				usage(argc, argv);
				return 1;
			}
			image = argv[i];
		}
	}

	ret = create_image(first_level, second_level, kernel_image, ramdisk,
			   buffer_size, image);
	return ret;
}
