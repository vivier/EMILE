/*
 *
 * (c) 2004 Laurent Vivier <Laurent@Vivier.EU>
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
#include "libconfig.h"
#include "device.h"

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_GETINFO = 'g',
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"getinfo",	1, NULL,	ARG_GETINFO	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [OPTION] <partition> <path>\n", argv[0]);
	fprintf(stderr, "Set/get the path to the configuration file\n");
	fprintf(stderr, "   -h, --help          display this text\n");
	fprintf(stderr, "   -g, --getinfo       "
	                "get second level information\n");
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "    %s /dev/sda2 (sd0,4)/emile.conf\n", argv[0]);
	fprintf(stderr, "sd -> SCSI disk\n");
	fprintf(stderr, "0  -> SCSI Id\n");
	fprintf(stderr, "4  -> partition /dev/sda5\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int set_conf(char* image, char *path)
{
	int fd;
	char property[256];
	int ret;
	device_io_t device;
	int driver;
	int disk;
	int partition;
	char dev_name[256];
	map_t *map;

	snprintf(property, 256, "configuration %s", path);

	fd = open(image, O_RDWR);

	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_second_set_configuration(fd, (int8_t *)property);
	close(fd);

	if (ret == -1) {
		fprintf(stderr, "Cannot set configuration\n");
		return 3;
	}

	/* compute driver new checksum */

	ret = emile_scsi_get_rdev(image, &driver, &disk, &partition);
	emile_get_dev_name(dev_name, driver, disk, 0);
	device_sector_size = 512;
        device.write_sector = (stream_write_sector_t)device_write_sector;
        device.read_sector = (stream_read_sector_t)device_read_sector;
        device.close = (stream_close_t)device_close;
        device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;
        device.data = (void*)device_open(dev_name, O_RDWR);

        map = map_open(&device);
        if (map == NULL)
        {
                fprintf(stderr, "ERROR: cannot open partition map\n");
                return 4;
        }

	ret = map_update_checksum(map, 0);
	if (ret == -1) {
                fprintf(stderr, "ERROR: cannot update driver checksum\n");
                return 5;
	}

	map_close(map);

	return 0;
}

int get_conf(char* image)
{
	int fd;
	int8_t *configuration;
	char property[256];

	fd = open(image, O_RDONLY);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	configuration = emile_second_get_configuration(fd);
	close(fd);

	if (configuration == NULL) {
		fprintf(stderr, "Cannot find configuration\n");
		return 3;
	} 

	if (config_get_property(configuration, "configuration", property) == -1) {
		fprintf(stderr, "Invalid configuration\n");
		return 4;
	}

	printf("%s\n", property);
	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	char *file = NULL;
	char *conf = NULL;
	int action_getinfo = 0;
	int option_index;
	int c;

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
		case ARG_GETINFO:
			action_getinfo = 1;
			break;
		}
	}
	if (optind < argc)
		file = argv[optind];
	optind++;
	if (optind < argc)
		conf = argv[optind];

	if (file == NULL)
	{
		fprintf(stderr,
			"ERROR: you must provide an image file or a block device.\n");
		usage(argc, argv);
		return 1;
	}

	if (action_getinfo)
		return get_conf(file);

	if (conf == NULL) {
		fprintf(stderr,
			"ERROR: you must provide an configuration path.\n");
		usage(argc, argv);
		return 1;
	}
	ret = set_conf(file, conf);

	return ret;
}
