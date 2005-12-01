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
static char *map_path = NULL;
static char *backup_path = NULL;
static char *partition = NULL;
static char *append_string = NULL;

enum {
	ACTION_NONE =		0x00000000,
	ACTION_SCANBUS = 	0x00000001,
	ACTION_SET_HFS = 	0x00000002,
	ACTION_RESTORE = 	0x00000004,
	ACTION_BACKUP = 	0x00000008,
	ACTION_APPEND = 	0x00000010,
	ACTION_TEST =		0x00000020,
	ACTION_FIRST =		0x00000040,
	ACTION_SECOND =		0x00000080,
	ACTION_KERNEL =		0x00000100,
	ACTION_PARTITION = 	0x00000200,
	ACTION_MAP =	 	0x00000400,
};

enum {
	ARG_NONE = 0,
	ARG_SCANBUS,
	ARG_SET_HFS,
	ARG_RESTORE,
	ARG_BACKUP,
	ARG_APPEND = 'a',
	ARG_VERBOSE ='v',
	ARG_TEST = 't',
	ARG_FIRST = 'f',
	ARG_SECOND = 's',
	ARG_KERNEL = 'k',
	ARG_PARTITION = 'p',
	ARG_HELP = 'h',
	ARG_MAP = 'm',
};

static struct option long_options[] =
{
	{"verbose",	0, NULL,	ARG_VERBOSE		},
	{"first",	1, NULL,	ARG_FIRST		},
	{"second",	1, NULL,	ARG_SECOND		},
	{"kernel",	1, NULL,	ARG_KERNEL		},
	{"map",		1, NULL,	ARG_MAP			},
	{"partition",	1, NULL,	ARG_PARTITION		},
	{"help",	0, NULL,	ARG_HELP		},
	{"scanbus",	0, NULL,	ARG_SCANBUS		},
	{"set-hfs",	0, NULL,	ARG_SET_HFS 		},
	{"restore",	2, NULL,	ARG_RESTORE 		},
	{"backup",	2,  NULL,	ARG_BACKUP	 	},
	{"test", 	0, NULL,	ARG_TEST 		},
	{"append", 	0, NULL,	ARG_APPEND 		},
	{NULL,		0, NULL,	0			}
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [OPTION]\n", argv[0]);
	fprintf(stderr, "Update and install EMILE stuff on your SCSI disk.\n");
	fprintf(stderr, "EMILE allows to boot linux directly from linux partition\n");
	fprintf(stderr,"  -h, --help           display this text\n");
	fprintf(stderr,"  -v, --verbose        active verbose mode\n");
	fprintf(stderr,"  -t, --test           active test mode (don't write to disk)\n");
	fprintf(stderr,"  --scanbus            display information about all disks and partitions\n");
	fprintf(stderr,"  -f, --first PATH     set path of EMILE first level\n");
	fprintf(stderr,"  -s, --second PATH    set path of EMILE second level\n");
	fprintf(stderr,"  -k, --kernel PATH    set path of kernel\n");
	fprintf(stderr,"  -m, --map PATH       set path to the EMILE kernel map file (generated)\n");
	fprintf(stderr,"  -a, --append ARG     set kernel command line\n");
	fprintf(stderr,"  -p, --partition DEV  define device where to install boot block\n");
	fprintf(stderr,"  --restore[=FILE]     save current boot block from FILE\n");
	fprintf(stderr,"  --backup[=FILE]      save current boot block to FILE\n");
	fprintf(stderr,"  --set-hfs            set type of partition DEV to Apple_HFS (needed to be bootable)\n");
	fprintf(stderr, "\nUse \"--test\" to see default values\n");
	fprintf(stderr, "!!! USE WITH CAUTION AND AT YOUR OWN RISK !!!\n");

	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int open_map_of( char *dev_name, int flags, 
			emile_map_t **map, int *partition)
{
	int ret;
	int disk;
	char disk_name[16];
	char *driver;

	ret = emile_scsi_get_rdev(dev_name, &driver, &disk, partition);
	if (ret == -1)
		return -1;

	sprintf(disk_name, "%s%c", driver, 'a' + disk);

	*map = emile_map_open(disk_name, flags);
	if (*map == NULL)
		return -1;

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

static int restore_bootblock(char *dev_name, char *filename)
{
	emile_map_t *map;
	int ret;
	int partition;
	char bootblock[BOOTBLOCK_SIZE];
	int fd;

	if (!check_is_EMILE_bootblock(dev_name))
	{
		fprintf(stderr, "ERROR: cannot restore bootblock over non-EMILE bootblock\n");
		return -1;
	}

	/* read bootblock */

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return -1;

	ret = read(fd, bootblock, BOOTBLOCK_SIZE);
	if (ret != BOOTBLOCK_SIZE)
		return -1;

	close(fd);

	/* write bootblock */

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

	ret = emile_map_set_partition_type(map, "Apple_HFS");
	if (ret == -1)
		return -1;

	ret = emile_map_partition_set_bootable(map, 1);
	if (ret == -1)
		return -1;

	ret = emile_map_write(map, partition - 1);
	if (ret == -1)
		return -1;

	emile_map_close(map);

	return 0;
}

int main(int argc, char **argv)
{
	int ret;
	int c;
	int option_index = 0;
	char tmp_partition[16];
	char tmp_append[512];
	int fd;
	int action = ACTION_NONE;

	while(1)
	{
		c = getopt_long(argc, argv, "vhtf:a:s:k:b:", long_options, &option_index);
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
		case ARG_SCANBUS:
			action |= ACTION_SCANBUS;
			break;
		case ARG_FIRST:
			action |= ACTION_FIRST;
			first_path = optarg;
			break;
		case ARG_SECOND:
			action |= ACTION_SECOND;
			second_path = optarg;
			break;
		case ARG_KERNEL:
			action |= ACTION_KERNEL;
			kernel_path = optarg;
			break;
		case ARG_MAP:
			action |= ACTION_MAP;
			map_path = optarg;
			break;
		case ARG_PARTITION:
			action |= ACTION_PARTITION;
			partition = optarg;
			break;
		case ARG_SET_HFS:
			action |= ACTION_SET_HFS;
			break;
		case ARG_RESTORE:
			action |= ACTION_RESTORE;
			if (optarg != NULL)
				backup_path = optarg;
			else
				backup_path = PREFIX "/boot/emile/bootblock.backup";
			break;
		case ARG_BACKUP:
			action |= ACTION_BACKUP;
			if (optarg != NULL)
				backup_path = optarg;
			else
				backup_path = PREFIX "/boot/emile/bootblock.backup";
			break;
		case ARG_APPEND:
			action |= ACTION_APPEND;
			append_string = optarg;
			break;
		case ARG_TEST:
			action |= ACTION_TEST;
			break;
		default:
			fprintf(stderr, "ERROR: unknown option %s (%d, %c)\n",
					argv[optind], c, c);
			return 1;
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

	/* seek first HFS partition */

	if (partition == NULL)
	{
		int fd;
		char dev_name[16];
		emile_map_t* map;
		char *part_type;
		int i;
		char *driver;
		int disk;
		int partnb;

		fd = open(second_path, O_RDONLY);
		if (fd == -1)
		{
			fprintf(stderr, "ERROR: cannot open \"%s\"\n", 
					second_path);
			return 2;
		}

		ret = emile_scsi_get_dev(fd, &driver, &disk, &partnb);
		if (ret == -1)
		{
			fprintf(stderr,"ERROR: cannot find device of \"%s\"\n", 
					second_path);
			return 2;
		}
		close(fd);

		sprintf(dev_name, "%s%c", driver, disk + 'a');

		 /* ROM boots on the first HFS partition it finds */

		map = emile_map_open(dev_name, O_RDONLY);
		if (map == NULL)
		{
			fprintf(stderr, "ERROR: cannot open partition map\n");
			return 2;
		}

		for (i = 0; i < emile_map_get_number(map); i++)
		{
			ret = emile_map_read(map, i);
			if (ret == -1)
				break;

			part_type = emile_map_get_partition_type(map);
			if (strcmp(part_type, "Apple_HFS") == 0)
			{
				sprintf(tmp_partition, "%s%d", dev_name, i + 1);
				partition = tmp_partition;
				break;
			}
		}
		emile_map_close(map);
	}

	/* if there is no HFS partition, we'll use the partition
	 * where there is second_scsi (/boot or /)
	 */

	if (partition == NULL)
	{
		int fd;
		char *driver;
		int disk;
		int partnb;

		fd = open(second_path, O_RDONLY);
		if (fd == -1)
			return 2;
		ret = emile_scsi_get_dev(fd, &driver, &disk, &partnb);
		if (ret == 0)
		{
			sprintf(tmp_partition, 
				"%s%c%d", driver, disk + 'a', partnb);
			partition = tmp_partition;
		}
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

	if (action & ACTION_RESTORE)
	{
		if (action & ~(ACTION_RESTORE | ACTION_PARTITION))
		{
			fprintf(stderr, 
	"ERROR: \"--restore\" cannot be used with other arguments\n");
			return 13;
		}

		ret = restore_bootblock(partition, backup_path);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot restore bootblock %s from %s\n", 
			partition, backup_path);
			return 14;
		}
		printf("Bootblock restore successfully done.\n");

		return 0;
	}

	if (append_string == NULL)
	{
		char *driver;
		int disk;
		int partnb;

		fd = open(second_path, O_RDONLY);
		if (fd == -1)
		{
			fprintf(stderr, "ERROR: cannot open \"%s\"\n", 
					second_path);
			return 2;
		}

		ret = emile_scsi_get_dev(fd, &driver, &disk, &partnb);
		if (ret == -1)
		{
			fprintf(stderr,"ERROR: cannot find device of \"%s\"\n", 
					second_path);
			return 2;
		}
		close(fd);

		sprintf(tmp_append, "root=%s%c%d", driver, disk + 'a', partnb);
		append_string = tmp_append;
	}

	ret = check_has_apple_driver(partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot check if Apple_Driver exists\n");
		fprintf(stderr, "       you should try as root\n");
		if ((action & ACTION_TEST) == 0)
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
		if ((action & ACTION_TEST) == 0)
			return 5;
	}

	ret = check_is_hfs(partition);
	if (ret == -1)
	{
		fprintf(stderr,
			"ERROR: cannot check if partition is Apple_HFS\n");
		fprintf(stderr, "       you should try as root\n");
		if ((action & ACTION_TEST) == 0)
			return 6;
	}
	if ( (ret == 0) && (action & ACTION_SET_HFS) )
	{
		fprintf(stderr,
	"ERROR: to be bootable a partition must be of type Apple_HFS\n");
		fprintf(stderr,
	"       you can change it to Apple_HFS using \"--set-hfs\" argument\n");
		if ((action & ACTION_TEST) == 0)
			return 7;
	}

	ret = check_is_EMILE_bootblock(partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot check bootblock type\n");
		fprintf(stderr, "       you should try as root\n");
		if ((action & ACTION_TEST) == 0)
			return 8;
	}
	if ( (ret == 0) && ((action & ACTION_BACKUP) == 0) )
	{
		fprintf(stderr,
	"ERROR: there is already a bootblock on \"%s\"\n", partition);
		fprintf(stderr,
	"       you must use \"--backup\" to save it\n");
		if ((action & ACTION_TEST) == 0)
			return 9;
	}
	
	if (action & ACTION_BACKUP)
	{
		if (action & ACTION_TEST)
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

	if (map_path == NULL)
	{
		map_path = (char*)malloc(strlen(kernel_path) + 5);
		if (map_path == NULL)
		{
			fprintf(stderr,
			"ERROR: cannot allocate memory\n");
			return 15;
		}
		sprintf(map_path, "%s.map", kernel_path);
	}

	printf("partition:   %s\n", partition);
	printf("first:       %s\n", first_path);
	printf("second:      %s\n", second_path);
	printf("kernel:      %s\n", kernel_path);
	printf("map file:    %s\n", map_path);
	printf("append:      %s\n", append_string);

	/* set kernel info into second level */

	fd = open(second_path, O_RDWR);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open \"%s\"\n",
				second_path);
		return 16;
	}

	if ((action & ACTION_TEST) == 0)
	{
		char *configuration;
		struct emile_container *container;
		short unit_id;
		char map_info[64];

		container = emile_second_create_mapfile(&unit_id, map_path, kernel_path);
		if (container == NULL)
		{
			fprintf(stderr, 
		"ERROR: cannot set \"%s\" information in \"%s\".\n", 
				kernel_path, map_path);
			return 17;
		}

		/* set second configuration */

		lseek(fd, 0, SEEK_SET);
		configuration = emile_second_get_configuration(fd);

		/* set kernel info */

		sprintf(map_info, "container:(sd%d)0x%x,0x%x", unit_id, 
				   container->blocks[0].offset, container->blocks[0].count);
		emile_second_set_property(configuration, "kernel", map_info);

		/* set cmdline */

		emile_second_set_property(configuration, "parameters", append_string);

		/* save configuration */

		lseek(fd, 0, SEEK_SET);
		ret = emile_second_set_configuration(fd, configuration);
		if (ret != 0)
		{
			free(configuration);
			fprintf(stderr,
		"ERROR: cannot set configuration in %s\n", second_path);
			return 19;
		}
		free(configuration);
	}

	close(fd);

	/* set second info in first level */

	fd = open(first_path, O_RDWR);
	if (fd == -1)
	{
		fprintf(stderr, 
			"ERROR: cannot open \"%s\".\n", first_path);
		return 20;
	}

	if ((action & ACTION_TEST) == 0)
	{
		ret = emile_first_set_param_scsi(fd, second_path);
		if (ret == -1)
		{
			fprintf(stderr, 
		"ERROR: cannot set \"%s\" information into \"%s\".\n", 
				second_path, first_path);
			return 21;
		}
	}

	close(fd);

	if ((action & ACTION_TEST) == 0)
	{
		/* copy first level to boot block */

		ret = copy_file_to_bootblock(first_path, partition);
		if (ret == -1)
		{
			fprintf(stderr,
		"ERROR: cannot write \"%s\" to bootblock of \"%s\".\n", 
					first_path, partition);
			fprintf(stderr,
		"       %s\n", strerror(errno));
			return 22;
		}

		/* set HFS if needed */

		if (action & ACTION_SET_HFS)
		{
			ret = set_HFS(partition);
			if (ret == -1)
			{
				fprintf( stderr, 
			"ERROR: cannot set partition type of \"%s\" to Apple_HFS.\n"
					, partition);
				return 23;
			}
		}
	}
	
	return 0;
}
