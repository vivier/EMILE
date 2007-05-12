/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
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
#include <libgen.h>

#include "libemile.h"
#include "emile_config.h"

int verbose = 0;

extern void scanbus(void);


enum {
	ACTION_NONE =		0x00000000,
	ACTION_SCANBUS = 	0x00000001,
	ACTION_SET_HFS = 	0x00000002,
	ACTION_RESTORE = 	0x00000004,
	ACTION_BACKUP = 	0x00000008,
	ACTION_TEST =		0x00000010,
	ACTION_CONFIG = 	0x00000020,
};

enum {
	ARG_NONE = 0,
	ARG_SCANBUS,
	ARG_SET_HFS,
	ARG_RESTORE,
	ARG_BACKUP,
	ARG_VERBOSE ='v',
	ARG_TEST = 't',
	ARG_HELP = 'h',
	ARG_CONFIG = 'c',
};

static struct option long_options[] =
{
	{"scanbus",	0, NULL,	ARG_SCANBUS		},
	{"set-hfs",	0, NULL,	ARG_SET_HFS 		},
	{"restore",	2, NULL,	ARG_RESTORE 		},
	{"backup",	2,  NULL,	ARG_BACKUP	 	},
	{"verbose",	0, NULL,	ARG_VERBOSE		},
	{"help",	0, NULL,	ARG_HELP		},
	{"test", 	0, NULL,	ARG_TEST 		},
	{"config", 	1, NULL,	ARG_CONFIG 		},
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
	fprintf(stderr,"  --restore[=FILE]     save current boot block from FILE\n");
	fprintf(stderr,"  --backup[=FILE]      save current boot block to FILE\n");
	fprintf(stderr,"  --set-hfs            set type of partition DEV to Apple_HFS (needed to be bootable)\n");
	fprintf(stderr,"  -c, --config FILE    use config file FILE\n");
	fprintf(stderr, "!!! USE WITH CAUTION AND AT YOUR OWN RISK !!!\n");

	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int open_map_of( char *dev_name, int flags, 
			emile_map_t **map, int *partition)
{
	int ret;
	int disk;
	char disk_name[16];
	int driver;

	ret = emile_scsi_get_rdev(dev_name, &driver, &disk, partition);
	if (ret == -1)
		return -2;

	emile_get_dev_name(disk_name, driver, disk, 0);

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
	if (ret < 0)
		return ret;

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
	if (ret < 0)
		return ret;

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
	if (ret < 0)
		return ret;

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
	if (ret < 0)
		return ret;

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
	if (ret < 0)
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
	if (ret < 0)
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
	if (ret < 0)
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
	char *backup_path = PREFIX "/boot/emile/bootblock.backup";
	char *config_path = PREFIX "/boot/emile/emile.conf";
	char *partition;
	char *first_path;
	char *second_path;
	char *kernel_path;
	char *initrd_path;
	char *kernel_map_path;
	char *initrd_map_path;
	char *append_string;
	int ret;
	int c;
	int option_index = 0;
	int fd;
	int action = ACTION_NONE;
	emile_config *config;

	while(1)
	{
		c = getopt_long(argc, argv, "vhtc:", long_options, &option_index);
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
		case ARG_SET_HFS:
			action |= ACTION_SET_HFS;
			break;
		case ARG_RESTORE:
			action |= ACTION_RESTORE;
			if (optarg != NULL)
				backup_path = optarg;
			break;
		case ARG_BACKUP:
			action |= ACTION_BACKUP;
			if (optarg != NULL)
				backup_path = optarg;
			break;
		case ARG_TEST:
			action |= ACTION_TEST;
			break;
		case ARG_CONFIG:
			action |= ACTION_CONFIG;
			config_path = optarg;
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

	/* read config file */

	config = emile_config_open(config_path);
	if (config == NULL)
	{
		fprintf(stderr, "ERROR: cannot open config file %s\n", config_path);
		return 2;
	}

	ret = emile_config_get(config, CONFIG_PARTITION, &partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: you must specify in %s a partition to set\n"
			        "       EMILE bootblock\n", config_path);
		fprintf(stderr,
	"       you can have the list of available partitions with \"--scanbus\".\n");
		emile_config_close(config);
		return 3;
	}

	if (action & ACTION_RESTORE)
	{
		char* new_name;

		if (action & ~ACTION_RESTORE)
		{
			fprintf(stderr, 
	"ERROR: \"--restore\" cannot be used with other arguments\n");
			emile_config_close(config);
			return 13;
		}

		ret = restore_bootblock(partition, backup_path);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot restore bootblock %s from %s\n", 
			partition, backup_path);
			emile_config_close(config);
			return 14;
		}
		printf("Bootblock restore successfully done.\n");

		/* rename backup file to .old */
		
		new_name = (char*)malloc(strlen(backup_path) + 4 + 1);

		sprintf(new_name, "%s.old", new_name);

		unlink(new_name);
		rename(backup_path, new_name);

		free(new_name);

		emile_config_close(config);
		return 0;
	}

	ret = check_has_apple_driver(partition);
	if (ret != -2)
	{
		if (ret == -1)
		{
			fprintf(stderr, "ERROR: cannot check if Apple_Driver exists\n");
			fprintf(stderr, "       you should try as root\n");
			if ((action & ACTION_TEST) == 0)
			{
				emile_config_close(config);
				return 4;
			}
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
			{
				emile_config_close(config);
				return 5;
			}
		}

		ret = check_is_hfs(partition);
		if (ret == -1)
		{
			fprintf(stderr,
				"ERROR: cannot check if partition is Apple_HFS\n");
			fprintf(stderr, "       you should try as root\n");
			if ((action & ACTION_TEST) == 0)
			{
				emile_config_close(config);
				return 6;
			}
		}
		if ( (ret == 0) && !(action & ACTION_SET_HFS) )
		{
			fprintf(stderr,
		"ERROR: to be bootable a partition must be of type Apple_HFS\n");
			fprintf(stderr,
		"       you can change it to Apple_HFS using \"--set-hfs\" argument\n");
			if ((action & ACTION_TEST) == 0)
			{
				emile_config_close(config);
				return 7;
			}
		}

		ret = check_is_EMILE_bootblock(partition);
		if (ret == -1)
		{
			fprintf(stderr, "ERROR: cannot check bootblock type\n");
			fprintf(stderr, "       you should try as root\n");
			if ((action & ACTION_TEST) == 0)
			{
				emile_config_close(config);
				return 8;
			}
		}
	}
	if ( (ret == 0) && ((action & ACTION_BACKUP) == 0) )
	{
		fprintf(stderr,
	"ERROR: there is already a bootblock on \"%s\"\n", partition);
		fprintf(stderr,
	"       you must use \"--backup\" to save it\n");
		if ((action & ACTION_TEST) == 0)
		{
			emile_config_close(config);
			return 9;
		}
	}
	
	if (action & ACTION_BACKUP)
	{
		if (action & ACTION_TEST)
		{
			fprintf(stderr, 
	"ERROR: \"--backup\" cannot be used with \"--test\"\n");
			emile_config_close(config);
			return 13;
		}

		ret = backup_bootblock(partition, backup_path);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot backup bootblock %s to %s\n", 
			partition, backup_path);
			emile_config_close(config);
			return 14;
		}
		printf("Bootblock backup successfully done.\n");
	}

	ret = emile_config_get(config, CONFIG_FIRST_LEVEL, &first_path);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read first level path from %s\n",
			config_path);
		emile_config_close(config);
		return 2;
	}

	ret = emile_config_get(config, CONFIG_SECOND_LEVEL, &second_path);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read second level path from %s\n",
			config_path);
		emile_config_close(config);
		return 2;
	}

	printf("partition:   %s\n", partition);
	printf("first:       %s\n", first_path);
	printf("second:      %s\n", second_path);

	ret = emile_config_read_first_entry(config);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read first kernel entry from %s\n",
			config_path);
		emile_config_close(config);
		return 2;
	}

	ret = emile_config_get(config, CONFIG_KERNEL, &kernel_path);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read kernel path from %s\n",
			config_path);
		emile_config_close(config);
		return 2;
	}

	if (!emile_is_url(kernel_path))
	{
		ret = emile_config_get(config, CONFIG_KERNEL_MAP, &kernel_map_path);
		if (ret == -1)
		{
			char *a = strdup(kernel_path);
			char *b = strdup(kernel_path);
			char *base = basename(a);
			char *dir = dirname(b);
			kernel_map_path = (char*)malloc(strlen(kernel_path) + 6);
			if (kernel_map_path == NULL)
			{
				fprintf(stderr, "ERROR: cannot allocate memory\n");
				emile_config_close(config);
				return 15;
			}
			sprintf(kernel_map_path, "%s/.%s.map", dir, base);
			free(a);
			free(b);
		}
	} else
		kernel_map_path = kernel_path;

	ret = emile_config_get(config, CONFIG_INITRD, &initrd_path);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read initrd path from %s\n",
			config_path);
		emile_config_close(config);
		return 2;
	}

	if (!emile_is_url(initrd_path))
	{
		ret = emile_config_get(config, CONFIG_INITRD_MAP, &initrd_map_path);
		if (ret == -1)
		{
			char *a = strdup(initrd_path);
			char *b = strdup(initrd_path);
			char *base = basename(a);
			char *dir = dirname(b);
			initrd_map_path = (char*)malloc(strlen(initrd_path) + 6);
			if (initrd_map_path == NULL)
			{
				fprintf(stderr,
				"ERROR: cannot allocate memory\n");
				emile_config_close(config);
				return 15;
			}
			sprintf(initrd_map_path, "%s/.%s.map", dir, base);
			free(a);
			free(b);
		}
	} else
		initrd_map_path = initrd_path;

	ret = emile_config_get(config, CONFIG_ARGS, &append_string);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot read kernel parameters from %s\n",
			config_path);
		emile_config_close(config);
		return 2;
	}

	printf("kernel:      %s\n", kernel_path);
	printf("append:      %s\n", append_string);
	printf("kernel map file:    %s\n", kernel_map_path);
	printf("initrd:     %s\n", initrd_path);
	printf("initrd map file:    %s\n", initrd_map_path);

	/* set kernel info into second level */

	fd = open(second_path, O_RDWR);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open \"%s\"\n",
				second_path);
		emile_config_close(config);
		return 16;
	}

	if ((action & ACTION_TEST) == 0)
	{
		struct emile_container *container;
		unsigned short unit_id;
		char kernel_map_info[64];
		char initrd_map_info[64];
		int drive, second, size;

		/* get block mapping of kernel in filesystem */

		container = emile_second_create_mapfile(&unit_id, kernel_map_path, kernel_path);
		if (container == NULL)
		{
			fprintf(stderr, 
		"ERROR: cannot set \"%s\" information in \"%s\".\n", 
				kernel_path, kernel_map_path);
			emile_config_close(config);
			return 17;
		}

		/* set kernel info */

		sprintf(kernel_map_info, "container:(sd%d)0x%x,0x%x", unit_id, 
				   container->blocks[0].offset, 
				   container->blocks[0].count);
		free(container);

		/* set second configuration */

		ret = emile_first_get_param(fd, &drive, &second, &size);
		if (ret == EEMILE_UNKNOWN_FIRST)
			lseek(fd, 0, SEEK_SET);

		/* get block mapping of initrd */

		container = emile_second_create_mapfile(&unit_id, initrd_map_path, initrd_path);
		if (container == NULL)
		{
			fprintf(stderr, 
		"ERROR: cannot set \"%s\" information in \"%s\".\n", 
				initrd_path, initrd_map_path);
			emile_config_close(config);
			return 17;
		}
		sprintf(initrd_map_info, "container:(sd%d)0x%x,0x%x", unit_id, 
				   container->blocks[0].offset, 
				   container->blocks[0].count);
		free(container);
		ret = emile_second_set_param(fd, kernel_map_info, 
					     append_string, initrd_map_info);
		if (ret != 0)
		{
			fprintf(stderr,
		"ERROR: cannot set \"%s\" information in \"%s\".\n", 
				initrd_path, initrd_map_path);
			emile_config_close(config);
			return 18;
		}
	}

	close(fd);

	/* set second info in first level */

	fd = open(first_path, O_RDWR);
	if (fd == -1)
	{
		fprintf(stderr, 
			"ERROR: cannot open \"%s\".\n", first_path);
		emile_config_close(config);
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
			emile_config_close(config);
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
			emile_config_close(config);
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
				emile_config_close(config);
				return 23;
			}
		}
	}
	
	emile_config_close(config);
	return 0;
}
