/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "libemile.h"

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_KERNEL = 'k',
	ARG_RAMDISK = 'r',
	ARG_APPEND = 'a',
	ARG_GETINFO = 'g',
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"kernel",	1, NULL,	ARG_KERNEL	},
	{"ramdisk",	1, NULL,	ARG_RAMDISK	},
	{"append",	1, NULL,	ARG_APPEND	},
	{"getinfo",	1, NULL,	ARG_GETINFO	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [OPTION] <file>\n", argv[0]);
	fprintf(stderr, "Configure second level to load kernel and ramdisk\n");
	fprintf(stderr, "   <file> can be a file or a device (/dev/fd0)\n");
	fprintf(stderr, "   -h, --help     display this text\n");
	fprintf(stderr, "   -k, --kernel   set kernel path\n");
	fprintf(stderr, "   -r, --ramdisk  set ramdisk path\n");
	fprintf(stderr, "   -a, --append   set kernel command line\n");
	fprintf(stderr, "   -g, --getinfo  get second level information\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int set_cmdline(char* image, char *kernel, char* cmdline, char *ramdisk)
{
	int fd;
	int ret;

	fd = open(image, O_RDWR);

	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_second_set_param(fd, kernel, cmdline, ramdisk);

	close(fd);

	return 0;
}

int get_cmdline(char* image)
{
	int fd;
	int ret;
	char kernel[255];
	char cmdline[255];
	char ramdisk[255];

	fd = open(image, O_RDONLY);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_second_get_param(fd, kernel, cmdline, ramdisk);

	if (kernel[0])
		printf("kernel path:  \"%s\"\n", kernel);
	else
		fprintf(stderr, "No kernel path found\n");
	if (cmdline[0])
		printf("command line: \"%s\"\n", cmdline);
	else
		fprintf(stderr, "No command line found\n");
	if (ramdisk[0])
		printf("ramdisk path: \"%s\"\n", ramdisk);
	else
		fprintf(stderr, "No ramdisk path found\n");

	close(fd);

	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	char *kernel = NULL;
	char *cmdline = NULL;
	char *ramdisk = NULL;
	char *file = NULL;
	int action_getinfo = 0;
	int option_index;
	int c;

	if (argc != 3)
	{
		usage(argc, argv);
		return 1;
	}

	while(1)
	{
		c = getopt_long(argc, argv, "ha:k:r:g", long_options,
				&option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		 case ARG_KERNEL:
			kernel = optarg;
			break;
		case ARG_APPEND:
			cmdline = optarg;
			break;
		case ARG_RAMDISK:
			ramdisk = optarg;
			break;
		case ARG_GETINFO:
			action_getinfo = 1;
			break;
		}
	}
	if (optind < argc)
		file = argv[optind];

	if (file == NULL)
	{
		fprintf(stderr,
			"ERROR: you must provide an image file or a block device.\n");
		usage(argc, argv);
		return 1;
	}

	if ( action_getinfo && ( kernel || cmdline || ramdisk) )
	{
		fprintf(stderr, "Incompatible parameters\n");
		return 1;
	}

	if (action_getinfo)
		ret = get_cmdline(file);
	else
		ret = set_cmdline(file, kernel, cmdline, ramdisk);

	return ret;
}
