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
static char *append_string = NULL;
static int buffer_size = 0;

enum {
	ARG_NONE = 0,
	ARG_SCANBUS,
	ARG_SET_HFS,
	ARG_SET_STARTUP,
	ARG_BACKUP,
	ARG_TEST,
	ARG_APPEND,
	ARG_VERBOSE ='v',
	ARG_FIRST = 'f',
	ARG_SECOND = 's',
	ARG_KERNEL = 'k',
	ARG_BUFFER = 'b',
	ARG_PARTITION = 'p',
	ARG_HELP = 'h',
};

static struct option long_options[] =
{
	{"verbose",	0, NULL,	ARG_VERBOSE		},
	{"first",	1, NULL,	ARG_FIRST		},
	{"second",	1, NULL,	ARG_SECOND		},
	{"kernel",	1, NULL,	ARG_KERNEL		},
	{"buffer",	1, NULL,	ARG_BUFFER		},
	{"partition",	1, NULL,	ARG_PARTITION		},
	{"help",	0, NULL,	ARG_HELP		},
	{"scanbus",	0, NULL,	ARG_SCANBUS		},
	{"set-hfs",	1, NULL,	ARG_SET_HFS 		},
	{"set-startup",	0, NULL,	ARG_SET_STARTUP 	},
	{"backup",	2,  NULL,	ARG_BACKUP	 	},
	{"test", 	0, NULL,	ARG_TEST 		},
	{"append", 	0, NULL,	ARG_APPEND 		},
	{NULL,		0, NULL,	0			}
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int open_map_of( char *dev_name, int flags, 
			emile_map_t **map, int *partition)
{
	int ret;
	int disk;
	char disk_name[16];

	ret = emile_scsi_get_rdev(dev_name, &disk, partition);
	if (ret == -1)
		return -1;

	sprintf(disk_name, "/dev/sd%c", 'a' + disk);

	*map = emile_map_open(disk_name, flags);

	return 0;
}

static int check_has_apple_driver(char *dev_name)
{
	emile_map_t *map;
	int partition;
	int ret;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret == -1)
		return -1;

	ret = emile_map_has_apple_driver(map);
	emile_map_close(map);

	return ret;
}

static int check_is_hfs(char *dev_name)
{
	emile_map_t *map;
	int ret;
	int partition;
	char *part_type;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret == -1)
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
	int partition;
	char bootblock[BOOTBLOCK_SIZE];
	int bootblock_type;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret == -1)
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
	int partition;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret == -1)
		return -1;

	ret = emile_map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = emile_map_partition_is_startup(map);

	emile_map_close(map);

	return ret;
}

static int backup_bootblock(char *dev_name, char *filename)
{
	emile_map_t *map;
	int ret;
	int partition;
	char bootblock[BOOTBLOCK_SIZE];
	int fd;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret == -1)
		return -1;

	ret = emile_map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = emile_map_bootblock_read(map, bootblock);
	if (ret == -1)
		return -1;

	emile_map_close(map);

	/* save bootblock */

	fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		if (errno == EEXIST)
		{
			fprintf(stderr, "ERROR: \"%s\" already exists.\n",
					filename);
		}
		return -1;
	}

	ret = write(fd, bootblock, BOOTBLOCK_SIZE);
	if (ret != BOOTBLOCK_SIZE)
		return -1;

	close(fd);

	return 0;
}

static int copy_file_to_bootblock(char* first_path, char* dev_name)
{
	emile_map_t *map;
	int ret;
	int partition;
	char bootblock[BOOTBLOCK_SIZE];
	int fd;

	/* read first level */

	fd = open(first_path, O_RDONLY);
	if (fd == -1)
		return -1;

	ret = read(fd, bootblock, BOOTBLOCK_SIZE);
	if (ret != BOOTBLOCK_SIZE)
		return -1;

	close(fd);

	/* write bootblock to partition */

	ret = open_map_of(dev_name, O_RDWR, &map, &partition);
	if (ret == -1)
		return -1;

	ret = emile_map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = emile_map_bootblock_write(map, bootblock);
	if (ret == -1)
		return -1;

	emile_map_close(map);

	return 0;
}

static int set_HFS(char *dev_name)
{
	emile_map_t *map;
	int ret;
	int partition;

	ret = open_map_of(dev_name, O_RDWR, &map, &partition);
	if (ret == -1)
		return -1;

	ret = emile_map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = emile_map_set_partition_type(map, "APPLE_HFS");
	if (ret == -1)
		return -1;

	ret = emile_map_write(map, partition - 1);
	if (ret == -1)
		return -1;

	emile_map_close(map);

	return 0;
}

static int set_startup(char *dev_name)
{
	int ret;
	int partition;
	int disk;
	char disk_name[16];

	ret = emile_scsi_get_rdev(dev_name, &disk, &partition);
	if (ret == -1)
		return -1;

	sprintf(disk_name, "/dev/sd%c", 'a' + disk);

	ret = emile_map_set_startup(disk_name, partition - 1);
	if (ret == -1)
		return -1;

	return 0;
}

int main(int argc, char **argv)
{
	int ret;
	int c;
	int option_index = 0;
	int action_scanbus = 0;
	int action_set_hfs = 0;
	int action_set_startup = 0;
	int action_backup = 0;
	int action_set_buffer = 0;
	int action_test = 0;
	char tmp_partition[16];
	char tmp_append[512];
	int fd;

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
		case ARG_BACKUP:
			action_backup = 1;
			if (optarg != NULL)
				backup_path = optarg;
			else
				backup_path = PREFIX "/boot/emile/bootblock.backup";
			break;
		case ARG_APPEND:
			append_string = optarg;
			break;
		case ARG_TEST:
			action_test = 1;
			break;
		}
	}

	if (action_scanbus) {

		if (action_test) {
			fprintf(stderr, 
	"ERROR: \"--scanbus\" cannot be used with \"--test\"\n");
			return 1;
		}

		scanbus();
		return 0;
	}

	if (partition == NULL)
	{
		int fd;
		fd = open(second_path, O_RDONLY);
		if (fd == -1)
			return 2;
		ret = emile_scsi_get_dev(tmp_partition, fd);
		if (ret != -1)
			partition = tmp_partition;
		close(fd);
	}

	if (partition == NULL)
	{
		fprintf(stderr, 
	"ERROR: you must specify a partition to set EMILE bootblock\n");
		fprintf(stderr,
	"       you can have the list of available partitions with \"--scanbus\".\n");
		return 3;
	}

	if (append_string == NULL)
	{
		sprintf(tmp_append, "root=%s", partition);
		append_string = tmp_append;
	}

	ret = check_has_apple_driver(partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot check if Apple_Driver exists\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return 4;
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
			return 5;
	}

	ret = check_is_hfs(partition);
	if (ret == -1)
	{
		fprintf(stderr,
			"ERROR: cannot check if partition is Apple_HFS\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return 6;
	}
	if ( (ret == 0) && (action_set_hfs == 0) )
	{
		fprintf(stderr,
	"ERROR: to be bootable a partition must be of type Apple_HFS\n");
		fprintf(stderr,
	"       you can change it to Apple_HFS using \"--set-hfs\" argument\n");
		if (action_test == 0)
			return 7;
	}

	ret = check_is_EMILE_bootblock(partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot check bootblock type\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return 8;
	}
	if ( (ret == 0) && (action_backup == 0) )
	{
		fprintf(stderr,
	"ERROR: there is already a bootblock on \"%s\"\n", partition);
		fprintf(stderr,
	"       you must use \"--backup\" to save it\n");
		if (action_test == 0)
			return 9;
	}
	
	ret = check_is_startup(partition);
	if (ret == -1)
	{
		fprintf(stderr, 
			"ERROR: cannot check if it is startup partition\n");
		fprintf(stderr, "       you should try as root\n");
		if (action_test == 0)
			return 10;
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
			return 11;
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
				return 12;
		}
	}

	if (action_backup)
	{
		if (action_test)
		{
			fprintf(stderr, 
	"ERROR: \"--backup\" cannot be used with \"--test\"\n");
			return 13;
		}

		ret = backup_bootblock(partition, backup_path);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot backup bootblock %s to %s\n", 
			partition, backup_path);
			return 14;
		}
		printf("Bootblock backup successfully done.\n");
	}

	printf("partition:   %s\n", partition);
	printf("first:       %s\n", first_path);
	printf("second:      %s\n", second_path);
	printf("kernel:      %s\n", kernel_path);
	printf("append:      %s\n", append_string);
	printf("buffer size: %d\n", buffer_size);

	if (action_test == 0)
	{
		/* set kernel info into second level */

		fd = open(second_path, O_RDWR);
		if (fd == -1)
		{
			fprintf(stderr, "ERROR: cannot open \"%s\"\n",
					second_path);
			return 15;
		}

		/* set kernel info */

		ret = emile_second_set_kernel_scsi(fd, kernel_path);
		if (ret == -1)
		{
			fprintf(stderr, 
		"ERROR: cannot set \"%s\" information in \"%s\".\n", 
				kernel_path, second_path);
			return 16;
		}

		/* set buffer size */

		lseek(fd, 0, SEEK_SET);
		ret = emile_second_set_buffer_size(fd, buffer_size);
		if (ret == -1)
		{
			fprintf(stderr, 
		"ERROR: cannot set buffer size in \"%s\".\n", second_path);
			return 17;
		}

		/* set cmdline */

		lseek(fd, 0, SEEK_SET);
		ret = emile_second_set_cmdline(fd, append_string);
		if (ret == -1)
		{
			fprintf(stderr,
		"ERROR: cannot set append string \"%s\" in \"%s\".\n", 
				append_string, second_path);
			return 18;
		}
		close(fd);

		/* set second info in first level */

		fd = open(first_path, O_RDWR);
		if (fd == -1)
		{
			fprintf(stderr, 
				"ERROR: cannot open \"%s\".\n", first_path);
			return 19;
		}

		ret = emile_first_set_param_scsi(fd, second_path);
		if (ret == -1)
		{
			fprintf(stderr, 
		"ERROR: cannot set \"%s\" information into \"%s\".\n", 
				second_path, first_path);
			return 20;
		}

		close(fd);

		/* copy first level to boot block */

		ret = copy_file_to_bootblock(first_path, partition);
		if (ret == -1)
		{
			fprintf(stderr,
		"ERROR: cannot write \"%s\" to bootblock of \"%s\".\n", 
					first_path, partition);
			fprintf(stderr,
		"       %s\n", strerror(errno));
			return 21;
		}

		/* set HFS if needed */

		if (action_set_hfs)
		{
			ret = set_HFS(partition);
			if (ret == -1)
			{
				fprintf( stderr, 
			"ERROR: cannot set partition type of \"%s\" to Apple_HFS.\n"
					, partition);
				return 22;
			}
		}

		/* set startup if needed */

		if (action_set_startup)
		{
			ret = set_startup(partition);
			if (ret == -1)
			{
				fprintf( stderr, 
			"ERROR: cannot set partition type of \"%s\" to Apple_HFS.\n"
					, partition);
				return 22;
			}
		}
	}
	
	return 0;
}
