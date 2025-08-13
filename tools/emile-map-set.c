/*
 *
 * (c) 2004 Laurent Vivier <Laurent@Vivier.EU>
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

#include "libmap.h"
#include "libemile.h"
#include "device.h"

int verbose = 0;

extern void scanbus(void);
extern void diskinfo(char*);


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
	fprintf(stderr, "Usage: %s [OPTIONS] [<disk>]\n", argv[0]);
	fprintf(stderr, "       %s [OPTIONS] [<partition>|<disk> <part number>]\n", argv[0]);
	fprintf(stderr, "   -h, --help             display this text\n");
	fprintf(stderr, "   -v, --verbose          verbose mode\n");
	fprintf(stderr, "       --scanbus          "
	                "scan all available SCSI devices\n");
	fprintf(stderr, "   -s, --startup          "
	                "set the startup partition\n");
	fprintf(stderr, "   -t, --type=TYPE        "
	                "set the type of the partition\n");
	fprintf(stderr, "   -f, --flags=FLAGS      "
	                "set the type of the partition\n");
	fprintf(stderr, "   -g, --get-driver=FILE  "
	                "get the driver from the partition\n");
	fprintf(stderr, "   -p, --put-driver=FILE  "
	                "put the driver to the partition\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int get_driver(map_t *map, int partition, char* appledriver)
{
	int driver;
	int block_size, block_count;
	int driver_number;
	int block, size, type, part;
	int partition_base, partition_size;
	int bootstart, bootsize, bootaddr, bootentry, checksum;
	char processor[16];
	unsigned char *code;
	int fd;
	int ret;

	map_read(map, partition);
	if (strncmp(map_get_partition_type(map), 
		    "Apple_Driver", strlen("Apple_Driver")) != 0)
	{
		fprintf(stderr, 
	"ERROR: the type of the partition must begin with \"Apple_Driver\"\n");
		return -1;
	}
	if (strcmp(map_get_partition_name(map), "Macintosh") != 0)
	{
		fprintf(stderr, 
		"ERROR: the name of the partition must be \"Macintosh\"\n");
		return -1;
	}

	map_geometry(map, &block_size, &block_count);
	printf("block size: %d\n", block_size);

	map_get_partition_geometry(map, &partition_base, &partition_size);
	printf("partition base: %d, size %d\n", partition_base, partition_size);

	driver_number = map_get_driver_number(map);
	if (driver_number == 0)
	{
		fprintf(stderr, "ERROR: no driver on this device\n");
		return -1;
	}

	for (driver = 0; driver < driver_number; driver++)
	{
		map_get_driver_info(map, driver, &block, &size, &type);
		part  = map_seek_driver_partition(map, 
						block * block_size / 512 );
		if (part == partition)
		{
			map_read(map, part);
			if (emile_is_apple_driver(map))
				break;
		}
		part  = map_seek_driver_partition(map, block);
		if (part == partition)
		{
			map_read(map, part);
			if (emile_is_apple_driver(map))
				break;
		}
	}
	if (part != partition)
	{
		fprintf(stderr, 
			"ERROR: cannot find partition in driver table\n");
		return -1;
	}
	printf("Found driver %d for partition %d\n", driver, partition + 1);
	printf("base: %d size: %d type: %d\n", block * block_size / 512, 
					     size * block_size / 512 , type);
	map_get_bootinfo(map, &bootstart, &bootsize, &bootaddr, 
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

	code = (unsigned char*)malloc(partition_size * 512);
	if (code == NULL)
	{
		fprintf(stderr, "ERROR: cannot malloc() to load driver in memory\n");
		return -1;
	}

	ret = map_read_sector(map, 0, (char*)code, partition_size * 512);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read driver (read())\n");
		free(code);
		return -1;
	}

	ret = map_checksum(code, bootsize);
	if (ret != checksum)
		fprintf(stderr, "WARNING: checksum is invalid (0x%x)\n",
				ret);
	else
		printf("Checksum OK\n");

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

static int put_driver(map_t *map, int partition, char* appledriver)
{
	int block_size, block_count;
	int fd;
	int ret;
	unsigned char* code;
	struct stat st;
	int driver_number;
	int block, count, checksum;

	map_read(map, partition);

	if (strncmp(map_get_partition_type(map), 
		    "Apple_Driver", strlen("Apple_Driver")) != 0)
	{
		fprintf(stderr, 
	"ERROR: the type of the partition must begin with \"Apple_Driver\"\n");
		return -1;
	}
	if (strcmp(map_get_partition_name(map), "Macintosh") != 0)
	{
		fprintf(stderr, 
		"ERROR: the name of the partition must be \"Macintosh\"\n");
		return -1;
	}

	map_geometry(map, &block_size, &block_count);

	/* read driver from file */

	fd = open(appledriver, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open file %s\n", appledriver);
		return -1;
	}

	ret = fstat(fd, &st);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot stat file %s\n", appledriver);
		return -1;
	}

	code = malloc(st.st_size);
	if (code == NULL)
	{
		fprintf(stderr, "ERROR: cannot malloc %jd\n", st.st_size);
		return -1;
	}

	ret = read(fd, code, st.st_size);

	close(fd);

	if (ret != st.st_size)
	{
		fprintf(stderr, "ERROR: cannot read file %s\n", appledriver);
		return -1;
	}

	/* compute driver checksum */

	checksum = map_checksum(code, st.st_size);
	printf("Driver checksum: 0x%x\n", checksum);

	/* write file in partition */

	ret = map_write_sector(map, 0, (char*)code, st.st_size);
	free(code);

	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot write driver (write())\n");
		return -1;
	}

	/* set bootinfo */

	map_set_bootinfo(map, 0, st.st_size, 0, 0, checksum, "68000");
	map_partition_set_flags(map, 0x17F);
	
	/* add driver in drivers list */

	driver_number = map_get_driver_number(map);
	if (driver_number == -1)
	{
		fprintf(stderr, "ERROR: cannot read drivers number\n");
		return -1;
	}
	if (driver_number != 1) {
		fprintf(stderr, "ERROR: cannot manage more than one driver\n");
		return -1;
	}

	ret = map_get_partition_geometry(map, &block, &count);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read partition geometry\n");
		return -1;
	}

	ret = map_set_driver_info(map, driver_number - 1,
					block / (block_size / 512) , 
					count / (block_size / 512), 1);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot set driver info\n");
		return -1;
	}

	ret = map_write(map, partition);
	if (ret != partition)
	{
		fprintf(stderr, "ERROR: cannot set partition information\n");
		return -1;
	}

	ret = map_block0_write(map);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot set drivers information\n");
		return -1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	map_t *map;
	int ret;
	int disk;
	int partition = 0;
	char *disk_name;
	char buffer[16];
	int driver;
	int action = ACTION_NONE;
	char *dev_name = NULL;
	char *appledriver = NULL;
	int c;
	int option_index;
	int flags;
	char *type;
	device_io_t device;
	int open_flags;

	open_flags = O_RDONLY;
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
			open_flags = O_RDWR;
			break;
		case ARG_TYPE:
			action |= ACTION_TYPE;
			type = optarg;
			break;
		case ARG_STARTUP:
			action |= ACTION_STARTUP;
			open_flags = O_RDWR;
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
			open_flags = O_RDWR;
			break;
		}
	}
	if (optind < argc)
		dev_name = argv[optind++];
	if (optind < argc)
	{
		partition = strtol(argv[optind++], NULL, 0);
		if (partition == 0)
		{
			fprintf(stderr,
	"ERROR: partition number cannot be 0 !\n");
			return 1;
		}
	}

	if ( !action && dev_name)
	{
		diskinfo(dev_name);
		return 0;
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

	if (dev_name == NULL)
	{
		fprintf(stderr, "ERROR: you must specify a device\n");
		return 1;
	}

	if (partition == 0)
		ret = emile_scsi_get_rdev(dev_name, &driver, &disk, &partition);
	else
		ret = emile_scsi_get_rdev(dev_name, &driver, &disk, NULL);

	if (ret == -1)
	{
		disk_name = dev_name;
		driver = 0;
		disk = 0;
	}
	else if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot find disk of %s\n", dev_name);
		return 1;
	}
	else
	{
		emile_get_dev_name(buffer, driver, disk, 0);
		disk_name = buffer;
	}

	if (partition == 0)
	{
		fprintf(stderr, 
			"ERROR: you must provide device of a partition\n");
		return 1;
	}

	device_sector_size = 512;
	device.write_sector = (stream_write_sector_t)device_write_sector;
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;
	device.data = (void*)device_open(disk_name, open_flags);

	map = map_open(&device);
	if (map == NULL)
	{
		fprintf(stderr, "ERROR: cannot open partition map\n");
		return 4;
	}

	if (action & ACTION_STARTUP)
	{
		if (action & ~ACTION_STARTUP)
		{
			fprintf(stderr, 
			"ERROR: don't use --startup with other flags\n");
			return 2;
		}

		ret = map_set_startup(map, partition - 1);
		if (ret == -1)
			return 3;
	}

	if (action & ACTION_GET)
	{
		if (appledriver == NULL) {
			fprintf(stderr, "ERROR: filename missing\n");
			map_close(map);
			return 6;
		}

		ret = get_driver(map, partition - 1, appledriver);
		if (ret == -1) {
			fprintf(stderr, 
		"ERROR: cannot put driver from partition %d to file %s\n", 
				partition, appledriver);
			map_close(map);
			return 6;
		}

		map_close(map);
		return 0;
	}

	if (action & ACTION_PUT)
	{
		if (appledriver == NULL) {
			fprintf(stderr, "ERROR: filename missing\n");
			map_close(map);
			return 6;
		}

		ret = put_driver(map, partition - 1, appledriver);
		if (ret == -1) {
			fprintf(stderr, 
		"ERROR: cannot put driver to partition %d from file %s\n", 
				partition, appledriver);
			map_close(map);
			return 6;
		}

		map_close(map);
		return 0;
	}


	ret = map_read(map, partition - 1);
	if (ret != partition - 1)
	{
		fprintf(stderr, 
			"ERROR: cannot read partition %d info\n", partition);
		return 5;
	}

	if (action & ACTION_FLAGS)
	{
		map_partition_set_flags(map, flags);
	}

	if (action & ACTION_TYPE)
	{
		ret = map_set_partition_type(map, type);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot set partition type to %s\n", type);
			return 7;
		}
	}

	ret = map_write(map,  partition - 1);
	if (ret != partition - 1)
	{
		fprintf(stderr, 
			"ERROR: cannot write partition %d info\n", partition);
		return 8;
	}

	map_close(map);

	return 0;
}
