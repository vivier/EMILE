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
#include <string.h>
#include <unistd.h>

#include "libemile.h"

int verbose = 0;

extern void scanbus(void);


enum {
	ACTION_NONE =	 0x00,
	ACTION_FLAGS =	 0x01,
	ACTION_TYPE = 	 0x02,
	ACTION_STARTUP = 0x04,
	ACTION_SCANBUS = 0x08,
	ACTION_GET =	 0x10,
	ACTION_PUT =	 0x20,
};

enum {
	ARG_NONE = 0,
	ARG_SCANBUS,
	ARG_HELP = 'h',
	ARG_FLAGS = 'f',
	ARG_TYPE ='t',
	ARG_STARTUP ='s',
	ARG_VERBOSE = 'v',
	ARG_GET = 'g',
	ARG_PUT = 'p',
};

static struct option long_options[] =
{
	{"help",        0, NULL,        ARG_HELP        },
	{"flags",	1, NULL,	ARG_FLAGS	},
	{"type",	1, NULL,	ARG_TYPE	},
	{"startup",	0, NULL,	ARG_STARTUP	},
	{"scanbus",	0, NULL,	ARG_SCANBUS	},
	{"verbose",	0, NULL,	ARG_VERBOSE	},
	{"get-driver",	1, NULL,	ARG_GET		},
	{"put-driver",	1, NULL,	ARG_PUT		},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [--verbose|-v]--scanbus\n", argv[0]);
	fprintf(stderr, "Usage: %s [--startup|--flags FLAGS][--type TYPE][--get-driver|-g FILE][--put-driver|-p FILE] <partition>\n", argv[0]);
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int get_driver(emile_map_t *map, int partition, char* appledriver)
{
	int driver;
	int block_size, block_count;
	int driver_number;
	int block, size, type, part;
	int bootstart, bootsize, bootaddr, bootentry, checksum;
	char processor[16];
	char *code;
	int fd;
	int ret;

	emile_map_read(map, partition);
	if (strncmp(emile_map_get_partition_type(map), 
		    "Apple_Driver", strlen("Apple_Driver")) != 0)
	{
		fprintf(stderr, 
	"ERROR: the type of the partition must begin with \"Apple_Driver\"\n");
		return -1;
	}
	if (strcmp(emile_map_get_partition_name(map), "Macintosh") != 0)
	{
		fprintf(stderr, 
		"ERROR: the name of the partition must be \"Macintosh\"\n");
		return -1;
	}

	emile_map_geometry(map, &block_size, &block_count);

	driver_number = emile_map_get_driver_number(map);
	if (driver_number == 0)
	{
		fprintf(stderr, "ERROR: no driver on this device\n");
		return -1;
	}

	for (driver = 0; driver < driver_number; driver++)
	{
		emile_map_get_driver_info(map, driver, &block, &size, &type);
		part  = emile_map_seek_driver_partition(map, 
						block * block_size / 512 );
		if (part == partition)
			break;
	}
	if (part != partition)
	{
		fprintf(stderr, 
			"ERROR: cannot find partition in driver table\n");
		return -1;
	}
	printf("Found driver %d for partition %d\n", driver, partition + 1);
	printf("base: %d size: %d type: 0x%x\n", block, size, type);
	emile_map_get_bootinfo(map, &bootstart, &bootsize, &bootaddr, 
				    &bootentry, &checksum, processor);
	printf("Bootstart: %d, Bootsize: %d, Bootaddr: %d, Bootentry: %d\n",
		bootstart, bootsize, bootaddr, bootentry);
	printf("Checksum: 0x%04x, Processor: %s\n", checksum, processor);

	if (strcmp(processor, "68000") != 0)
	{
		fprintf( stderr, 
		"ERROR: cannot manage processor %s (not 68000)\n", processor);
		return -1;
	}

	code = (char*)malloc(bootsize);
	if (code == NULL)
	{
		fprintf(stderr, "ERROR: cannot malloc() to load driver in memory\n");
		return -1;
	}

	fd = open(emile_map_dev(map), O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot read driver (open())\n");
		free(code);
		return -1;
	}
	ret = lseek(fd, block * block_size, SEEK_SET);
	if (ret != block * block_size)
	{
		fprintf(stderr, "ERROR: cannot read drivera (lseek())\n");
		free(code);
		close(fd);
		return -1;
	}

	ret = read(fd, code, bootsize);
	close(fd);

	if (ret != bootsize)
	{
		fprintf(stderr, "ERROR: cannot read driver (read())\n");
		free(code);
		return -1;
	}

	if (emile_checksum(code, bootsize) != checksum)
		fprintf(stderr, "WARNING: checksum is invalid (0x%x)\n",
				emile_checksum(code, bootsize));

	fd = open(appledriver, O_WRONLY | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open %s to save driver\n",
				appledriver);
		free(code);
		return -1;
	}

	ret = write(fd, code, bootsize);
	close(fd);
	free(code);

	if (ret != bootsize)
	{
		fprintf(stderr, "ERROR: cannot save driver to %s\n", 
				appledriver);
		return -1;
	}
	return 0;
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
	char *appledriver = NULL;
	int c;
	int option_index;
	int flags;
	char *type;

	while(1)
	{
		c = getopt_long(argc, argv, "hvg:p:sf:t:", long_options,
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
		case ARG_GET:
			action |= ACTION_GET;
			appledriver = optarg;
			break;
		case ARG_PUT:
			action |= ACTION_PUT;
			appledriver = optarg;
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
	if ((action & ACTION_GET) && (action & ACTION_PUT)) {
		fprintf(stderr, "You should use --get-driver OR --put-driver\n");
		return 1;
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
		if (ret == -1)
			return 3;
	}

	if (action & ACTION_GET)
	{
		map = emile_map_open(disk_name, O_RDONLY);
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

		if (appledriver == NULL) {
			fprintf(stderr, "ERROR: filename missing\n");
			emile_map_close(map);
			return 6;
		}

		ret = get_driver(map, partition - 1, appledriver);
		if (ret == -1) {
			fprintf(stderr, 
		"ERROR: cannot put driver from partition %d to file %s\n", 
				partition, appledriver);
			emile_map_close(map);
			return 6;
		}

		emile_map_close(map);
		return 0;
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
			return 7;
		}
	}

	ret = emile_map_write(map,  partition - 1);
	if (ret != partition - 1)
	{
		fprintf(stderr, 
			"ERROR: cannot write partition %d info\n", partition);
		return 8;
	}

	emile_map_close(map);

	return 0;
}
