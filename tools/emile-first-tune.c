/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "libemile.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [-d <drive>][-o <offset>][-s <size>] <image>\n", argv[0]);
	fprintf(stderr, "\n     set first level boot block info\n");
	fprintf(stderr, "     -d <drive>  : set the drive number (default 1)\n");
	fprintf(stderr,	"     -o <offset> : set offset of second level in bytes\n");
	fprintf(stderr,	"     -s <size>   : set size of second level in bytes\n");
	fprintf(stderr, "Display current values if no flags provided\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int first_tune( char* image, unsigned short tune_mask, int drive_num, 
		int second_offset, int second_size)
{
	int fd;
	int ret;

	fd = open(image, O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_first_set_param(fd, tune_mask, drive_num, second_offset, second_size);

	close(fd);
	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	int cargc;
	char** cargv;
	char* image;
	unsigned short tune_mask;
	int drive_num, second_offset, second_size;

	tune_mask = 0;
	image = NULL;
	cargc = argc - 1;
	cargv = argv + 1;
	while (cargc > 0)
	{
		if (strcmp(*cargv, "-d") == 0)
		{
			tune_mask |= EMILE_FIRST_TUNE_DRIVE;
			cargv++;
			cargc--;
			if (cargv == 0)
			{
				fprintf(stderr, "-d needs drive number\n");
				usage(argc, argv);
				return 1;
			}
			drive_num = atoi(*cargv);
			cargv++;
			cargc--;
		}
		else if (strcmp(*cargv, "-o") == 0)
		{
			tune_mask |= EMILE_FIRST_TUNE_OFFSET;
			cargv++;
			cargc--;
			if (cargv == 0)
			{
				fprintf(stderr, "-o needs offset\n");
				usage(argc, argv);
				return 1;
			}
			second_offset = atoi(*cargv);
			second_offset = (second_offset + 0x1FF) & 0xFFFFFE00;
			cargv++;
			cargc--;
		}
		else if (strcmp(*cargv, "-s") == 0)
		{
			tune_mask |= EMILE_FIRST_TUNE_SIZE;
			cargv++;
			cargc--;
			if (cargv == 0)
			{
				fprintf(stderr, "-s needs size\n");
				usage(argc, argv);
				return 1;
			}
			second_size = atoi(*cargv);
			second_size = (second_size + 0x1FF) & 0xFFFFFE00;
			cargv++;
			cargc--;
		}
		else
		{
			if (image != NULL)
			{
				fprintf(stderr, "Duplicate filename %s %s\n",
					image, *cargv);
				usage(argc, argv);
				return 1;
			}

			image = *cargv;
			cargv++;
			cargc--;
		}
	}

	if (image == NULL)
	{
		fprintf(stderr, "Missing filename to apply tuning\n");
		usage(argc, argv);
		return 1;
	}

	ret = first_tune( image, tune_mask, drive_num, second_offset, second_size);

	return ret;
}
