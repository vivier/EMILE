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
#include <getopt.h>

#include "libemile.h"

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_DRIVE = 'd',
	ARG_OFFSET ='o',
	ARG_SIZE = 's',
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"drive",	1, NULL,	ARG_DRIVE	},
	{"offset",	1, NULL,	ARG_OFFSET	},
	{"size",	1, NULL,	ARG_SIZE	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [-d <drive>][-o <offset>][-s <size>] <image>\n", argv[0]);
	fprintf(stderr, "Set EMILE first level boot block info\n");
	fprintf(stderr, "   -d, --drive <drive>   set the drive number (default 1)\n");
	fprintf(stderr,	"   -o, --offset <offset> set offset of second level in bytes\n");
	fprintf(stderr,	"   -s, --size <size>     set size of second level in bytes\n");
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
	if (tune_mask == 0)
	{
		ret = emile_first_get_param(fd, &drive_num, &second_offset,
					    &second_size);
		if (ret == 0)
		{
			printf("EMILE boot block identified\n\n");
			printf("Drive number: %d\n", drive_num);
			printf("Second level offset: %d\n", second_offset);
			printf("Second level size: %d\n", second_size);
		}
		else
			printf("EMILE is not installed in this bootblock\n");

		return 0;
	}

	ret = emile_first_set_param(fd, tune_mask, drive_num, second_offset, second_size);

	close(fd);
	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	int option_index;
	int c;
	char* image = NULL;
	unsigned short tune_mask = 0;
	int drive_num, second_offset, second_size;

	while(1)
	{
		c = getopt_long(argc, argv, "hd:o:f:", long_options,
				&option_index);
		if (c == EOF)
			break;

		switch(c)
		{
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_DRIVE:
			tune_mask |= EMILE_FIRST_TUNE_DRIVE;
			drive_num = atoi(optarg);
			break;
		case ARG_OFFSET:
			tune_mask |= EMILE_FIRST_TUNE_OFFSET;
			second_offset = atoi(optarg);
			second_offset = (second_offset + 0x1FF) & 0xFFFFFE00;
			break;
		case ARG_SIZE:
			tune_mask |= EMILE_FIRST_TUNE_SIZE;
			second_size = atoi(optarg);
			second_size = (second_size + 0x1FF) & 0xFFFFFE00;
			break;
		}
	}

	if (optind < argc)
		image = argv[optind];

	if (image == NULL)
	{
		fprintf(stderr, "ERROR: Missing filename to apply tuning\n");
		usage(argc, argv);
		return 1;
	}

	ret = first_tune( image, tune_mask, drive_num, second_offset, second_size);
	switch(ret)
	{
	case 0:
		break;
	case EEMILE_CANNOT_WRITE_FIRST:
		fprintf(stderr, "ERROR: cannot write to file\n");
		break;
	case EEMILE_UNKNOWN_FIRST:
		fprintf(stderr, "ERROR: unknown file format\n");
		break;
	default:
		fprintf(stderr, "ERROR: unknowm error :-P\n");
		break;
	}

	return ret;
}
