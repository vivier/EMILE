/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "libemile.h"

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_VERBOSE = 'v',
	ARG_FIRST = 'f',
	ARG_SECOND = 's',
	ARG_KERNEL = 'k',
	ARG_RAMDISK = 'r',
	ARG_GETINFO = 'g',
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"verbose",	0, NULL,	ARG_VERBOSE	},
	{"first",	1, NULL,	ARG_FIRST	},
	{"second",	1, NULL,	ARG_SECOND	},
	{"kernel",	1, NULL,	ARG_KERNEL	},
	{"ramdisk",	1, NULL,	ARG_RAMDISK	},
	{"getinfo",	1, NULL,	ARG_GETINFO	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [OPTION] <image>\n", argv[0]);
	fprintf(stderr, "Create an EMILE bootable floppy disk\n");
	fprintf(stderr, "EMILE allows you to boot linux from a floppy disk\n");
	fprintf(stderr, "   -h, --help      display this text\n");
	fprintf(stderr, "   -v, --verbose   verbose mode\n");
	fprintf(stderr, "   -f, --first     first level to copy to floppy\n");
	fprintf(stderr, "   -s, --second    second level to copy to floppy\n");
	fprintf(stderr, "   -k, --kernel    kernel to copy to floppy\n");
	fprintf(stderr, "   -r, --ramdisk   ramdisk to copy to floppy\n");
	fprintf(stderr, "   -g, --getinfo   get information from >image>\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int main(int argc, char** argv)
{
	int verbose = 0;
	int option_index = 0;
	char* first_level = NULL;
	char* second_level = NULL;
	char* kernel_image = NULL;
	char* ramdisk = NULL;
	char* image = NULL;
	int action_getinfo = 0;
	int c;
	int ret;

	while(1)
	{
		c = getopt_long(argc, argv, "hvf:s:k:r:g", long_options, 
				&option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_VERBOSE:
			verbose = 1;
			break;
		case ARG_FIRST:
			first_level = optarg;
			break;
		case ARG_SECOND:
			second_level = optarg;
			break;
		case ARG_KERNEL:
			kernel_image = optarg;
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
		image = argv[optind];

	if (image == NULL)
	{
		fprintf(stderr, 
		"ERROR: you must provide an image file or a block device.\n");
		usage(argc, argv);
		return 1;
	}

	if (action_getinfo)
	{
		int fd;
		int drive_num;
		int second_offset;
		int second_size;

		fd = open(image, O_RDONLY);
		if (fd == -1)
		{
			fprintf(stderr, "ERROR: cannot open \"%s\"\n",
					image);
			return 2;
		}

		/* first level info */

		ret = emile_first_get_param(fd, &drive_num, &second_offset,
					    &second_size);

		if (ret == 0)
		{
			char * configuration;

			printf("EMILE boot block identified\n\n");
			printf("Drive number:        %d\n", drive_num);
			printf("Second level offset: %d\n", second_offset);
			printf("Second level size:   %d\n", second_size);

			/* second level info */

			configuration = emile_second_get_configuration(fd);
			if (configuration == NULL)
			{
				fprintf(stderr, "ERROR: cannot read second level\n");
				return 3;
			}
			printf("%s\n", configuration);

			free(configuration);
		}
		else
			printf("EMILE is not installed in this bootblock\n");

		close(fd);
		return 0;
	}

	if (first_level == NULL)
		first_level = PREFIX "/lib/emile/first_floppy";

	if (second_level == NULL)
		second_level = PREFIX "/lib/emile/second_floppy";

	if (kernel_image == NULL)
		kernel_image = PREFIX "/boot/vmlinuz";

	if (verbose)
	{
		printf("first:   %s\n", first_level);
		printf("second:  %s\n", second_level);
		printf("kernel:  %s\n", kernel_image);
		printf("ramdisk: %s\n", ramdisk);
		printf("image:   %s\n", image);
	}

	ret = emile_floppy_create_image(first_level, second_level, 
					kernel_image, ramdisk, image);
	switch(ret)
	{
	case 0:
		break;
	case EEMILE_CANNOT_WRITE_FIRST:
		fprintf(stderr, "ERROR: cannot write first\n");
		break;
	case EEMILE_CANNOT_WRITE_SECOND:
		fprintf(stderr, "ERROR: cannot write second\n");
		break;
	case EEMILE_CANNOT_WRITE_KERNEL:
		fprintf(stderr, "ERROR: cannot write kernel\n");
		break;
	case EEMILE_CANNOT_WRITE_RAMDISK:
		fprintf(stderr, "ERROR: cannot write ramdisk\n");
		break;
	case EEMILE_CANNOT_WRITE_PAD:
		fprintf(stderr, "ERROR: cannot write padding\n");
		break;
	case EEMILE_CANNOT_CREATE_IMAGE:
		fprintf(stderr, "ERROR: cannot create image\n");
		break;
	case EEMILE_CANNOT_OPEN_FILE:
		fprintf(stderr, "ERROR: cannot open one of provided files\n");
		break;
	default:
		fprintf(stderr, "ERROR: unknown error :-P\n");
		break;
	}

	return ret;
}
