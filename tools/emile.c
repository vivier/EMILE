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

#include <libconfig.h>

#include "libemile.h"
#include "libmap.h"
#include "emile_config.h"

int verbose = 0;

extern void scanbus(void);

static enum {
	ACTION_NONE =		0x00000000,
	ACTION_SCANBUS = 	0x00000001,
	ACTION_SET_HFS = 	0x00000002,
	ACTION_RESTORE = 	0x00000004,
	ACTION_BACKUP = 	0x00000008,
	ACTION_TEST =		0x00000010,
	ACTION_CONFIG = 	0x00000020,
} action = ACTION_NONE;

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
			map_t **map, int *partition)
{
	int ret;
	int disk;
	char disk_name[16];
	int driver;

	ret = emile_scsi_get_rdev(dev_name, &driver, &disk, partition);
	if (ret == -1)
		return -2;

	emile_get_dev_name(disk_name, driver, disk, 0);

	*map = map_open(disk_name, flags);
	if (*map == NULL)
		return -1;

	return 0;
}

static int check_has_apple_driver(char *dev_name)
{
	map_t *map;
	int partition;
	int ret;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret < 0)
		return ret;

	ret = map_has_apple_driver(map);
	map_close(map);

	return ret;
}

static int check_is_hfs(char *dev_name)
{
	map_t *map;
	int ret;
	int partition;
	char *part_type;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret < 0)
		return ret;

	ret = map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	part_type = map_get_partition_type(map);
	ret = (strcmp("Apple_HFS", part_type) == 0);

	map_close(map);

	return ret;
}

static int check_is_EMILE_bootblock(char *dev_name)
{
	map_t *map;
	int ret;
	int partition;
	char bootblock[BOOTBLOCK_SIZE];
	int bootblock_type;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret < 0)
		return ret;

	ret = map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = map_bootblock_read(map, bootblock);
	if (ret == -1)
		return -1;

	bootblock_type = map_bootblock_get_type(bootblock);

	map_close(map);

	return EMILE_BOOTBLOCK == bootblock_type;
}

static int backup_bootblock(char *dev_name, char *filename)
{
	map_t *map;
	int ret;
	int partition;
	char bootblock[BOOTBLOCK_SIZE];
	int fd;

	ret = open_map_of(dev_name, O_RDONLY, &map, &partition);
	if (ret < 0)
		return ret;

	ret = map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = map_bootblock_read(map, bootblock);
	if (ret == -1)
		return -1;

	map_close(map);

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
	map_t *map;
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

	ret = map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = map_bootblock_write(map, bootblock);
	if (ret == -1)
		return -1;

	map_close(map);

	return 0;
}

static int copy_file_to_bootblock(char* first_path, char* dev_name)
{
	map_t *map;
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

	ret = map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = map_bootblock_write(map, bootblock);
	if (ret == -1)
		return -1;

	map_close(map);

	return 0;
}

static int set_HFS(char *dev_name)
{
	map_t *map;
	int ret;
	int partition;

	ret = open_map_of(dev_name, O_RDWR, &map, &partition);
	if (ret < 0)
		return -1;

	ret = map_read(map, partition - 1);
	if (ret == -1)
		return -1;

	ret = map_set_partition_type(map, "Apple_HFS");
	if (ret == -1)
		return -1;

	ret = map_partition_set_bootable(map, 1);
	if (ret == -1)
		return -1;

	ret = map_write(map, partition - 1);
	if (ret == -1)
		return -1;

	map_close(map);

	return 0;
}

static char *get_map_name(char *filename)
{
	char *a, *b;
	char *base, *dir;
	char *map_name;

	a = strdup(filename);
	base = basename(a);

	b = strdup(filename);
	dir = dirname(b);

	map_name = (char*)malloc(strlen(filename) + 6);
	if (map_name == NULL)
		return NULL;
	sprintf(map_name, "%s/.%s.map", dir, base);
	free(a);
	free(b);

	return map_name;
}


static int add_file(int8_t *configuration,
		    char *index, char *property, char *path, char *map_path)
{
	struct emile_container *container;
	unsigned short unit_id;
	char map_info[64];

	if (emile_is_url(path))
	{
		if (verbose)
			printf("    %s %s\n", property, path);

		config_set_indexed_property(configuration,
					    "title", index,
					    property, path);
		return 0;
	}

	if ((action & ACTION_TEST) != 0)
	{
		map_path = tempnam(NULL, "emile-map_path-");
	}
	else if (map_path == NULL)
	{
		map_path = get_map_name(path);
		if (map_path == NULL)
			return -1;
	}
	else
		map_path = strdup(map_path);

	/* get block mapping of kernel in filesystem */

	container = emile_second_create_mapfile(&unit_id, map_path, path);
	if (container == NULL)
	{
		free(map_path);
		return -1;
	}

	sprintf(map_info, 
		"container:(sd%d)0x%x,0x%x", unit_id, 
		container->blocks[0].offset, 
		container->blocks[0].count);

	free(container);

	if (verbose)
		printf("    kernel %s (%s = %s)\n", path, map_path, map_info);

	config_set_indexed_property(configuration,
				    "title", index,
				    property, map_info);

	if ((action & ACTION_TEST) != 0)
		fprintf(stderr, "WARNING: test mode, you must remove manually %s\n", map_path);

	free(map_path);
	return 0;
}

static int8_t *set_config(emile_config *config, int drive)
{
	int default_entry;
	int gestaltid;
	int timeout;
	char *kernel_path;
	char *initrd_path;
	char *kernel_map_path;
	char *initrd_map_path;
	char *output;
	char *append_string;
	char *title;
	char buf[16];
	int ret;
	int8_t *configuration;

	configuration = malloc(65536);
	if (configuration == NULL)
	{
		fprintf(stderr, 
			"ERROR: cannot allocate memory for configuration\n");
		return NULL;
	}

	configuration[0] = 0;

	if (!emile_config_get(config, CONFIG_GESTALTID, &gestaltid))
	{      	 
		sprintf(buf, "%d", gestaltid);
		config_set_property(configuration, "gestaltID", buf);
	}

	if (!emile_config_get(config, CONFIG_DEFAULT, &default_entry))
	{       
		sprintf(buf, "%d", default_entry); 
		config_set_property(configuration, "default", buf);
	}

	if (!emile_config_get(config, CONFIG_TIMEOUT, &timeout))
	{       
		sprintf(buf, "%d", timeout);
		config_set_property(configuration, "timeout", buf);
	}

	if (!emile_config_get(config, CONFIG_VGA, &output))
		config_set_property(configuration, "vga", output);

	if (!emile_config_get(config, CONFIG_MODEM, &output))
		config_set_property(configuration, "modem", output);

	if (!emile_config_get(config, CONFIG_PRINTER, &output))
		config_set_property(configuration, "printer", output);

	emile_config_read_first_entry(config);

	do {
		if (!emile_config_get(config, CONFIG_TITLE, &title))
			config_add_property(configuration, "title", title);
		if (verbose)
			printf("title %s\n", title);

		if (!emile_config_get(config, CONFIG_KERNEL, &kernel_path))
		{
			ret = emile_config_get(config, CONFIG_KERNEL_MAP, 
					       &kernel_map_path);

			ret = add_file(configuration, title, 
					"kernel", kernel_path, 
					ret == -1 ? NULL : kernel_map_path);
			if (ret == -1)
			{
				fprintf(stderr, 
					"ERROR: cannot add kernel %s\n",
					kernel_path);
				free(configuration);
				return NULL;
			}
		}
		else
			fprintf(stderr, 
				"WARNING: missing kernel entry for %s\n", title);

		if (!emile_config_get(config, CONFIG_INITRD, &initrd_path))
		{
			ret = emile_config_get(config, CONFIG_INITRD_MAP, &initrd_map_path);

			ret = add_file(configuration, title, "initrd", initrd_path, 
					ret == -1 ? NULL : initrd_map_path);
			if (ret == -1)
			{
				free(configuration);
				fprintf(stderr, 
					"ERROR: cannot add initrd %s\n",
					initrd_path);
				fprintf(stderr, 
				"ERROR: missing kernel entry for %s\n", title);
				return NULL;
			}
		}

		if (!emile_config_get(config, CONFIG_ARGS, &append_string))
		{
			config_set_indexed_property(configuration,
							"title", title,
							"parameters", append_string);
			if (verbose)
				printf("    parameters %s\n", append_string);
		}
	} while (!emile_config_read_next(config));

	if (strlen((char*)configuration) > 1023)
	{
		int fd;
		char* bootconfig = "/boot/emile/.bootconfig";

		/* do not fit in second paramstring */

		fd = creat(bootconfig, S_IWUSR);
		if (fd == -1)
		{
			free(configuration);
			fprintf(stderr, 
			"ERROR: cannot create /boot/emile/.bootconfig\n");
			return NULL;
			
		}

		write(fd, configuration, strlen((char*)configuration) + 1);
		close(fd);
		free(configuration);

		configuration = malloc(1024);
		if (configuration == NULL)
		{
			fprintf(stderr, 
			"ERROR: cannot allocate memory for configuration\n");
			return NULL;
		}
		ret = add_file(configuration, NULL, 
				 "configuration", bootconfig, NULL);
		if (ret == -1)
		{
			free(configuration);
			fprintf(stderr, 
			"ERROR: cannot add %s to configuration\n", bootconfig);
			return NULL;
		}
	}
	return configuration;
}

int main(int argc, char **argv)
{
	char *backup_path = PREFIX "/boot/emile/bootblock.backup";
	char *config_path = PREFIX "/boot/emile/emile.conf";
	char *first_path;
	char *second_path;
	char *partition;
	int ret;
	int c;
	int option_index = 0;
	emile_config *config;
	int drive, second, size;
	int fd;
	int8_t *configuration;

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
		return 2;

	ret = emile_config_get(config, CONFIG_SECOND_LEVEL, &second_path);
	if (ret == -1)
		return 2;

	fd = open(first_path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, 
			"ERROR: cannot open \"%s\".\n", first_path);
		emile_config_close(config);
		return 20;
	}

	ret = emile_first_get_param(fd, &drive, &second, &size);

	close(fd);

	configuration = set_config(config, drive);
	if (ret)
		return ret;

	if ((action & ACTION_TEST) == 0)
	{
		/* set configuration in second level */

		fd = open(second_path, O_RDWR);
		emile_second_set_configuration(fd, configuration);
		close(fd);

		/* set second info in first level */

		fd = open(first_path, O_RDWR);

		ret = emile_first_set_param_scsi(fd, second_path);
		if (ret == -1)
		{
			fprintf(stderr, 
		"ERROR: cannot set \"%s\" information into \"%s\".\n", 
				second_path, first_path);
			emile_config_close(config);
			return 21;
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
