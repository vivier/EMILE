/*
 *
 * (c) 2004 Laurent Vivier <Laurent@Vivier.EU>
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

#include "emile.h"
#include "libemile.h"
#include "bootblock.h"

#define BLOCK_SIZE	512

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_DRIVE = 'd',
	ARG_OFFSET ='o',
	ARG_SIZE = 's',
	ARG_PATH = 'p',
	ARG_SCSI = 'i',
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"drive",	1, NULL,	ARG_DRIVE	},
	{"offset",	1, NULL,	ARG_OFFSET	},
	{"size",	1, NULL,	ARG_SIZE	},
	{"path",	1, NULL,	ARG_PATH	},
	{"scsi",	0, NULL,	ARG_SCSI	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [OPTIONS] <image>\n", argv[0]);
	fprintf(stderr, "\nSet EMILE first level boot block info:\n");
	fprintf(stderr, "   -h, --help           display this text\n");
	fprintf(stderr, "   -d, --drive=DRIVE    "
	                "set the drive number (default 1)\n");
	fprintf(stderr,	"   -o, --offset=OFFSET  "
	                "set offset of second level in bytes\n");
	fprintf(stderr,	"   -s, --size=SIZE      "
	                "set size of second level in bytes\n");
	fprintf(stderr, "   -i, --scsi           "
	                "specify scsi first level format\n"
			"                        (OFFSET is a block number)\n");
	fprintf(stderr, "   -p, --path=PATH      set path of second level\n"
	                "                        (SCSI only)\n");
	fprintf(stderr, "\nDisplay current values if no flag is provided\n");
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

int first_tune_path( char* image, char *second_path)
{
	int fd;
	int ret;

	fd = open(image, O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_first_set_param_scsi(fd, second_path);

	close(fd);
	return ret;
}

int first_tune_scsi( char* image, int drive_num, int second_offset, int size)
{
	int fd;
	int ret;

	fd = open(image, O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	if (drive_num == -1)
	{
		eBootBlock_t firstBlock;
		int ret;
		unsigned short *max_blocks = (unsigned short*)(((char*)&firstBlock) + 1022);
		unsigned long *second_size = (unsigned long*)(((char*)&firstBlock) + 1018);
		unsigned short *unit_id = (unsigned short*)(((char*)&firstBlock) + 1016);
		unsigned short *block_size = (unsigned short*)(((char*)&firstBlock) + 1014);
		char *ptr = ((char*)&firstBlock) + 1012;

		ret = read(fd, &firstBlock, sizeof(firstBlock));
		if (ret != sizeof(firstBlock))
		{
			printf("ERROR: cannot read bootblock\n");
			return 1;
		}
		if ( strncmp( (char*)firstBlock.boot_block_header.SysName+1,
			      "Mac Bootloader", 14) != 0 )
		{
			printf("ERROR: not an EMILE bootblock\n");
			return 2;
		}
		printf("Container size : %d\n", *max_blocks);
		printf("Second size    : %ld\n", *second_size);
		printf("Unit id        : %d\n", *unit_id);
		printf("Block size     : %d\n", *block_size);
		printf("Extents        :\n");
		while (*(short*)ptr != 0)
		{
			printf("(%d,", *(short*)ptr);
			ptr -= 4;
			printf("%ld) ", *(unsigned long*)ptr);
			ptr -= 2;
		}
		putchar('\n');
	}
	else
		ret = emile_first_set_param_scsi_extents(fd, drive_num, 
							 second_offset, 
							 size, BLOCK_SIZE);

	close(fd);

	return ret;
}

int main(int argc, char** argv)
{
	int ret;
	int option_index;
	int c;
	char* image = NULL;
	char* path = NULL;
	unsigned short tune_mask = 0;
	int drive_num = -1, second_offset, second_size;
	int use_scsi = 0;

	while(1)
	{
		c = getopt_long(argc, argv, "hd:o:s:p:i", long_options,
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
			break;
		case ARG_SIZE:
			tune_mask |= EMILE_FIRST_TUNE_SIZE;
			second_size = atoi(optarg);
			break;
		case ARG_PATH:
			path = optarg;
			break;
		case ARG_SCSI:
			use_scsi = 1;
			break;
		}
	}

	if (optind < argc)
		image = argv[optind];

	if (path && tune_mask)
	{
		fprintf(stderr, "ERROR: you cannot supply second path and size, offset or drive number\n");
		usage(argc, argv);
		return 1;
	}

	if (image == NULL)
	{
		fprintf(stderr, "ERROR: Missing filename to apply tuning\n");
		usage(argc, argv);
		return 1;
	}

	if (path)
		ret = first_tune_path( image, path);
	else if (use_scsi)
		ret = first_tune_scsi( image, drive_num, second_offset, second_size);
	else {
		second_offset = (second_offset + 0x1FF) & 0xFFFFFE00;
		second_size = (second_size + 0x1FF) & 0xFFFFFE00;
		ret = first_tune( image, tune_mask, drive_num, second_offset, second_size);
	}
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
