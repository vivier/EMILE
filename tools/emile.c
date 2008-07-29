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
#include "device.h"

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
	ACTION_NO_FS =		0x00000040,
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
	ARG_NO_FS = 'n',
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
	{"no-fs", 	0, NULL,	ARG_NO_FS 		},
	{NULL,		0, NULL,	0			}
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [OPTION]\n", argv[0]);
	fprintf(stderr, "Update and install EMILE stuff on your SCSI disk.\n");
	fprintf(stderr, "EMILE allows to boot linux directly from linux partition\n");
	fprintf(stderr,"  -h, --help        display this text\n");
	fprintf(stderr,"  -v, --verbose     active verbose mode\n");
	fprintf(stderr,"  -t, --test        active test mode (don't write to disk)\n");
	fprintf(stderr,"  --scanbus         "
	               "display information about all disks and partitions\n");
	fprintf(stderr,"  --restore[=FILE]  restore boot block from FILE\n");
	fprintf(stderr,"  --backup[=FILE]   save current boot block to FILE\n");
	fprintf(stderr,"  --set-hfs         "
	               "set type of partition DEV to Apple_HFS\n"
	               "                    (needed to be bootable)\n");
	fprintf(stderr,"  -c, --config FILE use config file FILE\n");
	fprintf(stderr,"  -n, --no-fs       "
	               "don't use EMILE embededed filesystems\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int open_map_of( char *dev_name, int flags, 
			map_t **map, int *partition)
{
	int ret;
	int disk;
	char disk_name[16];
	int driver;
	device_io_t device;

	ret = emile_scsi_get_rdev(dev_name, &driver, &disk, partition);
	if (ret == -1)
		return -2;

	emile_get_dev_name(disk_name, driver, disk, 0);

	device_sector_size = 512;
	device.data = (void*)device_open(disk_name, flags);
	device.write_sector = (stream_read_sector_t)device_write_sector;
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;

	*map = map_open(&device);
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
		printf("    %s %s (%s = %s)\n", property, path, map_path, map_info);

	config_set_indexed_property(configuration,
				    "title", index,
				    property, map_info);

	if ((action & ACTION_TEST) != 0)
		fprintf(stderr, "WARNING: test mode, you must remove manually %s\n", map_path);

	free(map_path);
	return 0;
}

static int8_t *set_config_no_fs(char *config_path)
{
	int8_t *conffile;
	int fd;
	int i;
	int current;
	int ret;
	struct stat st;
	int8_t *configuration;
	char property[1024];
	char property2[1024];
	char property3[1024];
	static char *prolog[] = {
		"gestaltID",
		"default",
		"timeout",
		"vga",
		"modem",
		"printer"
		};

	configuration = malloc(65536);
	if (configuration == NULL)
	{
		fprintf(stderr, 
			"ERROR: cannot allocate memory for configuration\n");
		return NULL;
	}

	memset(configuration, 0, 65536);

	/* open configuration file */

	fd = open(config_path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open %s\n", config_path);
		return NULL;
	}
	if (fstat(fd, &st) == -1)
	{
		fprintf(stderr, "ERROR: cannot fstat %s\n", config_path);
		return NULL;
	}
	conffile = (int8_t*)malloc(st.st_size);
	if (conffile == NULL)
	{
		fprintf(stderr, "ERROR: cannot malloc() %s\n", config_path);
		return NULL;
	}

	if (read(fd, conffile, st.st_size) != st.st_size)
	{
		fprintf(stderr, "ERROR: cannot read() %s\n", config_path);
		return NULL;
	}
	close(fd);

	/* copy prolog */

	for (i = 0; i < sizeof(prolog) / sizeof(char*); i++)
        {
                if (config_get_property(conffile,
                                        prolog[i],
					property) != -1)
                        config_set_property(configuration, prolog[i], property);
        }

	current = 0;
	while (1) {
		ret = config_get_property(conffile + current,
					  "title", property);
		if (ret == -1)
			break;
		config_add_property(configuration, "title", property);
		if (verbose)
			printf("title %s\n", property);

		current += ret;
		current = config_get_next_property(conffile, current,
						   NULL, NULL);

		ret = config_get_indexed_property(conffile,
						  "title",
						  property,
						  "chainloader",
						  property2);
		if (ret != -1)
		{
			if (emile_is_url(property2))
			{
				config_set_indexed_property(configuration,
						"title", property,
						"chainloader", property2);
			}
			else
			{
				int fd;
				unsigned short unit_id;
				struct emile_container *container;
				struct stat st;
				char *chainloader;

				fd = open(property2, O_RDONLY);
				if (fd == -1)
				{
					fprintf(stderr,
						"ERROR: cannot open %s\n",
						 property2);
					return NULL;
				}
				fstat(fd, &st);

				container = malloc(
						sizeof(struct emile_container) +
						sizeof(struct emile_block));
				if (container == NULL)
				{
					fprintf(stderr,
						"ERROR: cannot malloc container"
						"\n");
					close(fd);
					return NULL;
				}
				ret = emile_scsi_create_container(fd,
								  &unit_id,
								  container,
								  2);
				close(fd);
				if (ret == -1)
				{
					fprintf(stderr,
						"ERROR: cannot create container"
						"\n");
					free(container);
					return NULL;
				}
				chainloader = malloc(32);
				if (chainloader == NULL)
				{
					fprintf(stderr,
					  "ERROR: cannot malloc chainloader\n");
					free(container);
					return NULL;
				}
				sprintf(chainloader,
			                "block:(sd%d)0x%x,0x%lx", unit_id,
			                container->blocks[0].offset,
			                st.st_size);
				free(container);
				config_set_indexed_property(configuration,
						"title", property,
						"chainloader", chainloader);
				free(chainloader);
			}
		}

		ret = config_get_indexed_property(conffile,
						  "title",
						  property,
						  "kernel",
						  property2);
		if (ret != -1)
		{
			ret = config_get_indexed_property(conffile,
							  "title",
							  property,
							  "kernel_map",
							  property3);

			ret = add_file(configuration, property, 
					"kernel", property2, 
					ret == -1 ? NULL : property3);
			if (ret == -1)
			{
				fprintf(stderr, 
					"ERROR: cannot add kernel %s\n",
					property2);
				free(conffile);
				free(configuration);
				return NULL;
			}
		}
		else
			fprintf(stderr, 
				"WARNING: missing kernel entry for %s\n",
				property);

		ret = config_get_indexed_property(conffile,
						  "title",
						  property,
						  "initrd",
						  property2);
		if (ret != -1)
		{
			ret = config_get_indexed_property(conffile,
							  "title",
							  property,
							  "initrd_map",
							  property3);

			ret = add_file(configuration, property,
				       "initrd", property2,
					ret == -1 ? NULL : property3);
			if (ret == -1)
			{
				free(configuration);
				fprintf(stderr, 
					"ERROR: cannot add initrd %s\n",
					property3);
				fprintf(stderr, 
				"ERROR: missing kernel entry for %s\n", property);
				return NULL;
			}
		}

		ret = config_get_indexed_property(conffile,
						  "title",
						  property,
						  "args",
						  property2);
		if (ret != -1)
		{
			config_set_indexed_property(configuration,
						    "title", property,
						    "args", property2);
			if (verbose)
				printf("    args %s\n", property2);
		}
	}

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

static int8_t *set_config(char *config_path)
{
	int8_t *configuration;
	int driver, disk, partition;
	int fd, ret;

	fd = open(config_path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "ERROR: cannot open %s\n", config_path);
		return NULL;
	}
	ret = emile_scsi_get_dev(fd, &driver, &disk, &partition);
	close(fd);
	if (ret == -1)
	{
		fprintf(stderr, "cannot find partition and disk of %s\n",
			config_path);
		return NULL;
	}

	configuration = (int8_t*)malloc(65536);
	if (configuration == NULL)
	{
		fprintf(stderr, 
			"ERROR: cannot allocate memory for configuration\n");
		return NULL;
	}
	memset(configuration, 0, 65536);

	sprintf((char*)configuration, "configuration (sd%d,%d)%s\n",
		disk, partition - 1, config_path);
	if (verbose)
		printf("    configuration (sd%d,%d)%s\n",
		       disk, partition - 1, config_path);

	return configuration;
}

int main(int argc, char **argv)
{
	char property[1024];
	char *backup_path = PREFIX "/boot/emile/bootblock.backup";
	char *config_path = PREFIX "/boot/emile/emile.conf";
	char *first_path;
	char *second_path;
	int ret;
	int c;
	char *partition;
	int option_index = 0;
	int drive, second, size;
	int fd;
	int8_t *configuration;
	int8_t *config;
	struct stat st;

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
		case ARG_NO_FS:
			action |= ACTION_NO_FS;
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

	fd = open(config_path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr,
			"ERROR: cannot open config file %s\n", config_path);
		return 2;
	}
	if (fstat(fd, &st) == -1)
	{
		fprintf(stderr,
			"ERROR: cannot fstat file %s\n", config_path);
		return 2;
	}
	config = (int8_t*)malloc(st.st_size);
	if (config == NULL)
	{
		fprintf(stderr,
			"ERROR: cannot malloc %s\n", config_path);
		return 2;
	}
	if (read(fd, config, st.st_size) != st.st_size)
	{
		fprintf(stderr, "ERROR: cannot read() %s\n", config_path);
		return 5;
	}
	close(fd);

	/* get partition */

	ret = config_get_property(config, "partition", property);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: you must specify in %s a partition to set\n"
			        "       EMILE bootblock\n", config_path);
		fprintf(stderr,
	"       you can have the list of available partitions with \"--scanbus\".\n");
		free(config);
		return 3;
	}
	partition = strdup(property);

	if (action & ACTION_RESTORE)
	{
		char* new_name;

		if (action & ~ACTION_RESTORE)
		{
			fprintf(stderr, 
	"ERROR: \"--restore\" cannot be used with other arguments\n");
			free(config);
			return 13;
		}

		ret = restore_bootblock(partition, backup_path);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot restore bootblock %s from %s\n", 
			partition, backup_path);
			free(config);
			return 14;
		}
		printf("Bootblock restore successfully done.\n");

		/* rename backup file to .old */
		
		new_name = (char*)malloc(strlen(backup_path) + 4 + 1);

		sprintf(new_name, "%s.old", new_name);

		unlink(new_name);
		rename(backup_path, new_name);

		free(new_name);

		free(config);
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
				free(config);
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
				free(config);
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
				free(config);
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
				free(config);
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
				free(config);
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
			free(config);
			return 9;
		}
	}
	
	if (action & ACTION_BACKUP)
	{
		if (action & ACTION_TEST)
		{
			fprintf(stderr, 
	"ERROR: \"--backup\" cannot be used with \"--test\"\n");
			free(config);
			return 13;
		}

		ret = backup_bootblock(partition, backup_path);
		if (ret == -1)
		{
			fprintf(stderr, 
			"ERROR: cannot backup bootblock %s to %s\n", 
			partition, backup_path);
			free(config);
			return 14;
		}
		printf("Bootblock backup successfully done.\n");
	}

	ret = config_get_property(config, "first_level", property);
	if (ret == -1)
		return 2;
	first_path = strdup(property);

	ret = config_get_property(config, "second_level", property);
	if (ret == -1)
		return 2;
	second_path = strdup(property);

	fd = open(first_path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, 
			"ERROR: cannot open \"%s\".\n", first_path);
		free(config);
		return 20;
	}

	ret = emile_first_get_param(fd, &drive, &second, &size);

	close(fd);

	if ((action & ACTION_NO_FS) == 0)
		configuration = set_config(config_path);
	else
		configuration = set_config_no_fs(config_path);
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
			free(first_path);
			free(second_path);
			free(config);
			free(configuration);
			return 21;
		}
		free(second_path);

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
			free(first_path);
			free(config);
			free(configuration);
			return 22;
		}
		free(first_path);

		/* set HFS if needed */

		if (action & ACTION_SET_HFS)
		{
			ret = set_HFS(partition);
			if (ret == -1)
			{
				fprintf( stderr, 
			"ERROR: cannot set partition type of \"%s\" to Apple_HFS.\n"
					, partition);
				free(config);
				free(configuration);
				return 23;
			}
		}
	}
	
	free(partition);
	free(config);
	free(configuration);
	return 0;
}
