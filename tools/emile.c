/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#include "libemile.h"

int verbose = 0;

extern void scanbus(void);

static char *first_path = PREFIX "/boot/emile/first_scsi";
static char *second_path = PREFIX "/boot/emile/second_scsi";
static char *kernel_path = PREFIX "/boot/vmlinuz";
static char *backup_path = NULL;
static char *partition = NULL;
static int buffer_size = 0;

enum {
	ARG_VERBOSE ='v',
	ARG_FIRST = 'f',
	ARG_SECOND = 's',
	ARG_KERNEL = 'k',
	ARG_BUFFER = 'b',
	ARG_PARTITION = 'p',
	ARG_HELP = 'h',
	ARG_SCANBUS = 1,
	ARG_SET_HFS = 2,
	ARG_SET_STARTUP = 3,
	ARG_BACKUP_BOOTBLOCK = 4,
	ARG_TEST = 5,
};

static struct option long_options[] =
{
	{"verbose",	0, NULL,	ARG_VERBOSE},
	{"first",	1, NULL,	ARG_FIRST},
	{"second",	1, NULL,	ARG_SECOND},
	{"kernel",	1, NULL,	ARG_KERNEL},
	{"buffer",	1, NULL,	ARG_BUFFER},
	{"partition",	1, NULL,	ARG_PARTITION},
	{"help",	0, NULL,	ARG_HELP},
	{"scanbus",	0, NULL,	ARG_SCANBUS},
	{"set-hfs",	1, NULL,	ARG_SET_HFS },
	{"set-startup",	0, NULL,	ARG_SET_STARTUP },
	{"backup-bootblock", 2,  NULL,	ARG_BACKUP_BOOTBLOCK },
	{"test", 	0,  NULL,	ARG_TEST },
	{NULL,		0, NULL,	0}
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int check_has_apple_driver(char *dev_name)
{
	emile_map_t *map;
	int ret;
	int disk;
	int partition;
	char disk_name[16];

	ret = emile_scsi_get_rdev(dev_name, &disk, &partition);
	sprintf(disk_name, "/dev/sd%c", 'a' + disk);

	map = emile_map_open(disk_name, O_RDONLY);
	if (map == NULL)
		return -1;

	ret = emile_map_has_apple_driver(map);
	emile_map_close(map);

	return ret;
}

static int check_is_hfs(char *dev_name)
{
	emile_map_t *map;
	int ret;
	int disk;
	int partition;
	char disk_name[16];
	char *part_type;

	ret = emile_scsi_get_rdev(dev_name, &disk, &partition);
	sprintf(disk_name, "/dev/sd%c", 'a' + disk);

	map = emile_map_open(disk_name, O_RDONLY);
	if (map == NULL)
		return -1;

	ret = emile_map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	part_type = emile_map_get_partition_type(map);
	ret = (strcmp("Apple_HFS", part_type) == 0);

	emile_map_close(map);

	return ret;
}

static int check_is_EMILE_bootblock(char *dev_name)
{
	emile_map_t *map;
	int ret;
	int disk;
	int partition;
	char disk_name[16];
	char bootblock[BOOTBLOCK_SIZE];
	int bootblock_type;

	ret = emile_scsi_get_rdev(dev_name, &disk, &partition);
	sprintf(disk_name, "/dev/sd%c", 'a' + disk);

	map = emile_map_open(disk_name, O_RDONLY);
	if (map == NULL)
		return -1;

	ret = emile_map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = emile_map_bootblock_read(map, bootblock);
	if (ret == -1)
		return -1;

	bootblock_type = emile_map_bootblock_get_type(bootblock);

	emile_map_close(map);

	return EMILE_BOOTBLOCK == bootblock_type;
}

static int check_is_startup(char *dev_name)
{
	emile_map_t *map;
	int ret;
	int disk;
	int partition;
	char disk_name[16];

	ret = emile_scsi_get_rdev(dev_name, &disk, &partition);
	sprintf(disk_name, "/dev/sd%c", 'a' + disk);

	map = emile_map_open(disk_name, O_RDONLY);
	if (map == NULL)
		return -1;

	ret = emile_map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = emile_map_partition_is_startup(map);

	emile_map_close(map);

	return ret;
}

static int backup_bootblock(char *dev_name)
{
	return -1;
}

int main(int argc, char **argv)
{
	int ret;
	int c;
	int option_index = 0;
	int action_scanbus = 0;
	int action_set_hfs = 0;
	int action_set_startup = 0;
	int action_backup_bootblock = 0;
	int action_set_buffer = 0;
	int action_test = 0;
	char tmp_partition[16];

	while(1)
	{
		c = getopt_long(argc, argv, "vhf:s:k:b:", long_options, &option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case ARG_SCANBUS:
			action_scanbus = 1;
			break;
		case ARG_VERBOSE:
			verbose++;
			break;
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_FIRST:
			first_path = optarg;
			break;
		case ARG_SECOND:
			second_path = optarg;
			break;
		case ARG_KERNEL:
			kernel_path = optarg;
			break;
		case ARG_BUFFER:
			action_set_buffer = 1;
			buffer_size = atoi(optarg);
			break;
		case ARG_PARTITION:
			partition = optarg;
			break;
		case ARG_SET_HFS:
			action_set_hfs = 1;
			break;
		case ARG_SET_STARTUP:
			action_set_startup = 1;
			break;
		case ARG_BACKUP_BOOTBLOCK:
			action_backup_bootblock = 1;
			if (optarg != NULL)
				backup_path = optarg;
			else
				backup_path = PREFIX "/boot/emile/bootblock.backup";
			break;
		case ARG_TEST:
			action_test = 1;
			break;
		}
	}

	if (action_scanbus) {
		scanbus();
		return 0;
	}

	if (partition == NULL)
	{
		if (partition == NULL)
		{
			int fd;
			fd = open(second_path, O_RDONLY);
			if (fd == -1)
				return -1;
			ret = emile_scsi_get_dev(tmp_partition, fd);
			if (ret != -1)
				partition = tmp_partition;
			close(fd);
		}
	}

	ret = check_has_apple_driver(partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot check if Apple_Driver exists\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return -1;
	}
	if (ret == 0)
	{
		fprintf(stderr,
	"ERROR: to be bootable a disk must have an Apple Driver on it\n");
		fprintf(stderr,
	"       You must partition this disk with Apple Disk utility\n");
		fprintf(stderr,
	"       or wait a release of EMILE allowing you to add this driver\n");
		if (action_test == 0)
			return -1;
	}

	ret = check_is_hfs(partition);
	if (ret == -1)
	{
		fprintf(stderr,
			"ERROR: cannot check if partition is Apple_HFS\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return -1;
	}
	if ( (ret == 0) && (action_set_hfs == 0) )
	{
		fprintf(stderr,
	"ERROR: to be bootable a partition must be of type Apple_HFS\n");
		fprintf(stderr,
	"       you can change it to Apple_HFS using \"--set-hfs\" argument\n");
		if (action_test == 0)
			return -1;
	}

	ret = check_is_EMILE_bootblock(partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot check bootblock type\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return -1;
	}
	if ( (ret == 0) && (action_backup_bootblock == 0) )
	{
		fprintf(stderr,
	"ERROR: there is already a bootblock on \"%s\"\n", partition);
		fprintf(stderr,
	"       you must use \"--backup-bootblock\" to save it\n");
		if (action_test == 0)
			return -1;
	}
	
	ret = check_is_startup(partition);
	if (ret == -1)
	{
		fprintf(stderr, 
			"ERROR: cannot check if it is startup partition\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return -1;
	}
	if ( (ret == 0) && (action_set_startup == 0) )
	{
		fprintf(stderr,
	"ERROR: \"%s\" is not the startup partition, \n", partition);
		fprintf(stderr,
	"       you must use \"--set-startup\" to set it,\n");
		fprintf(stderr,
	"       you can use later \"emile-set-startup\" to change it\n");
		if (action_test == 0)
			return -1;
	}

	if (action_set_buffer == 0)
	{
		buffer_size = emile_get_uncompressed_size(kernel_path);
		if (buffer_size == -1)
		{
			fprintf(stderr, 
		"ERROR: cannot compute size of uncompressed kernel\n");
			fprintf(stderr,
		"       use \"--buffer <size>\" to set it or set path of gzip in PATH\n");
			fprintf(stderr,
		"       or check \"%s\" can be read\n", kernel_path);
			if (action_test == 0)
				return -1;
		}
	}

	if (action_backup_bootblock)
	{
		if (action_test)
		{
			fprintf(stderr, 
	"ERROR: \"--backup-bootblock\" cannot be used with \"--test\"\n");
			return -1;
		}

		ret = backup_bootblock(partition);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot backup bootblock %s to %s\n", 
			partition, backup_path);
			return -1;
		}
		printf("Bootblock backup successfully done.\n");
	}

	printf("first:       %s\n", first_path);
	printf("second:      %s\n", second_path);
	printf("kernel:      %s\n", kernel_path);
	printf("buffer size: %d\n", buffer_size);
	printf("partition:   %s\n", partition);
#if 0
	int fd;
	int ret;

	/* set kernel info in second level */

	fd = open(argv[2], O_RDWR);	/* second */
	if (fd == -1)
	{
		perror("Cannot open second stage");
		return 1;
	}

	/* set buffer size */

	printf("Setting buffer size to %d\n", atoi(argv[4]));
	ret = emile_second_set_buffer_size(fd, atoi(argv[4]));

	/* set cmdline */

	lseek(fd, 0, SEEK_SET);
	printf("Setting command line to %s\n", argv[5]);
	ret = emile_second_set_cmdline(fd, argv[5]);

	/* set kernel info */

	lseek(fd, 0, SEEK_SET);
	ret = emile_second_set_kernel_scsi(fd, argv[3]);

	close(fd);

	/* set second info in first level */

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open first stage");
		return 1;
	}

	ret = emile_first_set_param_scsi(fd, argv[2]);
	close(fd);
#endif
	
	return 0;
}
