/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include "libemile.h"

int verbose = 0;

extern void scanbus(void);


enum {
	ACTION_NONE =	 0x00,
	ACTION_FLAGS =	 0x01,
	ACTION_TYPE = 	 0x02,
	ACTION_STARTUP = 0x04,
	ACTION_SCANBUS = 0x08,
};

enum {
	ARG_NONE = 0,
	ARG_SCANBUS,
	ARG_HELP = 'h',
	ARG_FLAGS = 'f',
	ARG_TYPE ='t',
	ARG_STARTUP ='s',
	ARG_VERBOSE = 'v',
};

static struct option long_options[] =
{
	{"help",        0, NULL,        ARG_HELP        },
	{"flags",	1, NULL,	ARG_FLAGS	},
	{"type",	1, NULL,	ARG_TYPE	},
	{"startup",	0, NULL,	ARG_STARTUP	},
	{"scanbus",	0, NULL,	ARG_SCANBUS	},
	{"verbose",	0, NULL,	ARG_VERBOSE	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [--verbose|-v][--scanbus][--startup|--flags FLAGS][--type TYPE] <partition>\n", argv[0]);
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int main(int argc, char** argv)
{
	emile_map_t *map;
	int ret;
	int disk;
	int partition;
	char disk_name[16];
	char *driver;
	int action = ACTION_NONE;
	char *dev_name = NULL;
	int c;
	int option_index;
	int flags;
	char *type;

	while(1)
	{
		c = getopt_long(argc, argv, "hvsf:t:", long_options,
				&option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case ARG_VERBOSE:
			verbose++;
			break;
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_FLAGS:
			action |= ACTION_FLAGS;
			flags = strtol(optarg, NULL, 0);
			break;
		case ARG_TYPE:
			action |= ACTION_TYPE;
			type = optarg;
			break;
		case ARG_STARTUP:
			action |= ACTION_STARTUP;
			break;
		case ARG_SCANBUS:
			action |= ACTION_SCANBUS;
			break;
		}
	}
	if (action & ACTION_SCANBUS) {
		if (action & ~ACTION_SCANBUS) {
			fprintf(stderr,
	"ERROR: \"--scanbus\" cannot be used with other arguments\n");
			return 1;
		}

		scanbus();
		return 0;
	}

	if (optind < argc)
		dev_name = argv[optind];

	if (dev_name == NULL)
	{
		fprintf(stderr, "ERROR: you must specify a device\n");
		return 1;
	}

	ret = emile_scsi_get_rdev(dev_name, &driver, &disk, &partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot find disk of %s\n", dev_name);
		return 1;
	}

	if (partition == 0)
	{
		fprintf(stderr, 
			"ERROR: you must provide device of a partition\n");
		return 1;
	}

	sprintf(disk_name, "%s%c", driver, 'a' + disk);

	if (action & ACTION_STARTUP)
	{
		if (action & ~ACTION_STARTUP)
		{
			fprintf(stderr, 
			"ERROR: don't use --startup with other flags\n");
			return 2;
		}

		ret = emile_map_set_startup(disk_name, partition - 1);
		if (ret == -1) {
			fprintf(stderr, "ERROR: cannot set startup partition\n");
			return 3;
		}
	}

	map = emile_map_open(disk_name, O_RDWR);
	if (map == NULL)
	{
		fprintf(stderr, "ERROR: cannot open partition map\n");
		return 4;
	}

	ret = emile_map_read(map, partition - 1);
	if (ret != partition - 1)
	{
		fprintf(stderr, 
			"ERROR: cannot read partition %d info\n", partition);
		return 5;
	}

	if (action & ACTION_FLAGS)
	{
		emile_map_partition_set_flags(map, flags);
	}

	if (action & ACTION_TYPE)
	{
		ret = emile_map_set_partition_type(map, type);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot set partition type to %s\n", type);
			return 6;
		}
	}

	ret = emile_map_write(map,  partition - 1);
	if (ret != partition - 1)
	{
		fprintf(stderr, 
			"ERROR: cannot write partition %d info\n", partition);
		return 5;
	}

	emile_map_close(map);

	return 0;
}
