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
	ARG_BUFFER ='b',
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"verbose",	0, NULL,	ARG_VERBOSE	},
	{"first",	1, NULL,	ARG_FIRST	},
	{"second",	1, NULL,	ARG_SECOND	},
	{"kernel",	1, NULL,	ARG_KERNEL	},
	{"ramdisk",	1, NULL,	ARG_RAMDISK	},
	{"buffer",	1, NULL,	ARG_BUFFER	},
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
	fprintf(stderr, "   -b, --buffer    buffer size to decompress kernel\n");
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
	unsigned long buffer_size = 0;
	char* image = NULL;
	int c;
	int ret;

	while(1)
	{
		c = getopt_long(argc, argv, "hvf:s:k:r:b:", long_options, 
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
		case ARG_BUFFER:
			buffer_size = atol(optarg);
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

	if (first_level == NULL)
		first_level = PREFIX "/lib/emile/first_floppy";

	if (second_level == NULL)
		second_level = PREFIX "/lib/emile/second_floppy";

	if (kernel_image == NULL)
		kernel_image = PREFIX "/boot/vmlinuz";

	if (buffer_size == 0)
	{
		buffer_size = emile_get_uncompressed_size(kernel_image);
		if (buffer_size == -1)
		{
			fprintf(stderr,
		"ERROR: cannot compute size of uncompressed kernel\n");
			fprintf(stderr,
		"       use \"--buffer <size>\" to set it or set path of gzip in PATH\n");
			fprintf(stderr,
					"       or check \"%s\" can be read\n", kernel_image);
			return 2;
		}
	}

	if (verbose)
	{
		printf("first:   %s\n", first_level);
		printf("second:  %s\n", second_level);
		printf("kernel:  %s\n", kernel_image);
		printf("buffer:  %ld\n", buffer_size);
		printf("ramdisk: %s\n", ramdisk);
		printf("image:   %s\n", image);
	}

	ret = emile_floppy_create_image(first_level, second_level, 
					kernel_image, ramdisk, buffer_size, 
					image);
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
