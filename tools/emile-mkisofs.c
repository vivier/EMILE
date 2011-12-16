/*
 *
 * (c) 2004-2006 Laurent Vivier <Laurent@lvivier.info>
 *
 * Some parts from mkisofs/apple_driver.c, (c) James Pearson 17/5/98
 *
 * By extracting a driver from an Apple CD, you become liable to obey
 * Apple Computer, Inc. Software License Agreements.
 *
 */

#define DRIVER_SIZE	(64*1024)

#define FIRST_PATH	"/boot/emile/first_scsi"
#define SECOND_PATH	"/boot/emile/m68k-second_scsi"
#define DRIVER_PATH	"/lib/emile/apple_driver"

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <libmap.h>
#include <emile.h>
#include <libemile.h>
#include <libiso9660.h>
#include <libstream.h>

#include "device.h"

#define COMMAND "/usr/bin/mkisofs %s -hfs -joliet -R -boot-hfs-file %s -graft-points -o %s"

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_VERBOSE = 'v',
	ARG_FIRST = 'f',
	ARG_SECOND = 's',
	ARG_KERNEL = 'k',
	ARG_RAMDISK = 'r',
	ARG_APPEND = 'a',
	ARG_APPLEDRIVER = 'd',
	ARG_EMILEDRIVER = 'e',
	ARG_CONFIG = 'c',
};

static struct option long_options[] = 
{
	{"help",	0, NULL,	ARG_HELP	},
	{"verbose",	0, NULL,	ARG_VERBOSE	},
	{"first",	1, NULL,	ARG_FIRST	},
	{"second",	1, NULL,	ARG_SECOND	},
	{"kernel",	1, NULL,	ARG_KERNEL	},
	{"ramdisk",	1, NULL,	ARG_RAMDISK	},
	{"append",	1, NULL,	ARG_APPEND	},
	{"appledriver",	1, NULL,	ARG_APPLEDRIVER },
	{"emiledriver",	1, NULL,	ARG_EMILEDRIVER },
	{"config",	1, NULL,	ARG_CONFIG	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage %s [FLAGS] filename pathspec [pathspec ...]\n", 
			argv[0]);
	fprintf(stderr, "Create and EMILE bootable CDROM\n");
	fprintf(stderr, "   -h, --help              display this text\n");
	fprintf(stderr, "   -v, --verbose           verbose mode\n");
	fprintf(stderr, "   -f, --first=FILE        "
	                "first level to copy to CDROM\n");
	fprintf(stderr, "   -s, --second=FILE       "
	                "second level to copy to CDROM\n");
	fprintf(stderr, "   -k, --kernel=PATH       "
	                "path of the kernel on CDROM\n");
	fprintf(stderr, "   -r, --ramdisk=PATH      "
	                "path of ramdisk on CDROM\n");
	fprintf(stderr, "   -a, --append=ARG        "
	                "set kernel command line\n");
	fprintf(stderr, "   -d, --appledriver=FILE  "
	                "appledriver to copy to CDROM\n");
	fprintf(stderr, "   -e, --emiledriver=FILE  "
	                "emiledriver to copy to CDROM\n");
	fprintf(stderr, "   -c, --config=FILE       "
	                "path of configuration file\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

#define BLOCKSIZE	(2048)

static int create_apple_driver(char *temp, char *appledriver, char *first_level)
{
	struct DriverDescriptor block0;
	struct Partition map2048;
	struct Partition map512;
	FILE* fd;
	int fd_driver;
	char *buffer;
	unsigned char *driver;
	struct stat st;
	int driver_size;

	/* read apple driver */

	fd_driver = open(appledriver, O_RDONLY);
	if (fd_driver == -1)
	{
		fprintf(stderr, "Cannot open %s\n", appledriver);
		return -1;
	}
	fstat(fd_driver, &st);
	driver_size = ((st.st_size + DRIVER_SIZE - 1) / DRIVER_SIZE) *
	              DRIVER_SIZE;
	driver = malloc(driver_size);
	memset(driver, 0, driver_size);
	if (driver == NULL)
	{
		fprintf(stderr, "Cannot malloc %d bytes\n", driver_size);
		return -1;
	}
	read(fd_driver, driver, st.st_size);

	close(fd_driver);

	/*	HFS CD Label Block                              512 bytes
	 *      Driver Partition Map (for 2048 byte blocks)     512 bytes
	 *      Driver Partition Map (for 512 byte blocks)      512 bytes
	 *      Empty                                           512 bytes
	 *      Driver Partition                                N x 2048 bytes
	 *      HFS Partition Boot Block                        1024 bytes
	 */


	memset(&block0, 0, sizeof(block0));
	write_short((u_int16_t*)&block0.Sig, DD_SIGNATURE);
	write_short((u_int16_t*)&block0.BlkSize, BLOCKSIZE);
	write_long((u_int32_t*)&block0.BlkCount, 0);		// set by mkisofs
	write_short((u_int16_t*)&block0.DevType, 1);
	write_short((u_int16_t*)&block0.DevId, 1);
	write_long((u_int32_t*)&block0.Data, 0);
	write_short((u_int16_t*)&block0.DrvrCount, 1);

	write_long((u_int32_t*)&block0.DrvInfo[0].Block, 0);
	write_short((u_int16_t*)&block0.DrvInfo[0].Size, (driver_size + BLOCKSIZE - 1) / BLOCKSIZE);
	write_short((u_int16_t*)&block0.DrvInfo[0].Type, kDriverTypeMacSCSI);

	memset(&map512, 0, sizeof(map512));
	write_short((u_int16_t*)&map512.Sig, MAP_SIGNATURE);
	write_long((u_int32_t*)&map512.PartBlkCnt, (driver_size + 512 - 1) / 512);
	write_long((u_int32_t*)&map512.PyPartStart,0);
	strncpy(map512.PartName, "Macintosh", 32);
	if (first_level == NULL)
		strncpy(map512.PartType, APPLE_DRIVER_EMILE, 32);
	else
		strncpy(map512.PartType, APPLE_DRIVER43, 32);
	write_long((u_int32_t*)&map512.LgDataStart, 0);
	write_long((u_int32_t*)&map512.DataCnt, 0);
	write_long((u_int32_t*)&map512.PartStatus, kPartitionAUXIsValid | 
				   kPartitionAUXIsAllocated | 
				   kPartitionAUXIsInUse | 
				   kPartitionAUXIsBootValid | 
				   kPartitionAUXIsReadable | 
				   kPartitionAUXIsWriteable | 
				   kPartitionAUXIsBootCodePositionIndependent | 
				   kPartitionIsChainCompatible | 
				   kPartitionIsRealDeviceDriver);
	write_long((u_int32_t*)&map512.LgBootStart, 0);
	write_long((u_int32_t*)&map512.BootSize, st.st_size);
	write_long((u_int32_t*)&map512.BootAddr, 0);
	write_long((u_int32_t*)&map512.BootAddr2, 0);
	write_long((u_int32_t*)&map512.BootEntry, 0);
	write_long((u_int32_t*)&map512.BootEntry2, 0);
	write_long((u_int32_t*)&map512.BootCksum, emile_checksum(driver, st.st_size));
	strncpy(map512.Processor, "68000", 16);
	write_long((u_int32_t*)map512.Pad, kSCSICDDriverSignature);

	map2048 = map512;
	write_long((u_int32_t*)&map2048.PartBlkCnt, (driver_size + BLOCKSIZE - 1) / BLOCKSIZE);

	strcpy(temp, "/tmp/emile-mkisofs-XXXXXX");
	mkstemp(temp);
	fd = fopen(temp, "w");

	fwrite(&block0, 1, sizeof(block0), fd);
	fwrite(&map2048, 1, sizeof(map2048), fd);
	fwrite(&map512, 1, sizeof(map512), fd);
	memset(&map512, 0, sizeof(map512));
	fwrite(&map512, 1, sizeof(map512), fd);

	fwrite(driver, driver_size, 1, fd);
	free(driver);

	/* read and write bootblock */

	buffer = malloc(1024);
        if (first_level == NULL) {
		memset(buffer, 0, 1024);
		/* genisoimage needs the bootblock signature */
		buffer[0] = 0x4c;
		buffer[1] = 0x4b;
	} else {
		fd_driver = open(first_level, O_RDONLY);
		if (fd_driver == -1)
		{
			fprintf(stderr,
				"ERROR: Cannot open first level \"%s\"\n",
				first_level);
			return -1;
		}
		read(fd_driver, buffer, 1024);
		close(fd_driver);
	}

	fwrite(buffer, 1024, 1, fd);
	free(buffer);
	fclose(fd);

	return 0;
}

static int get_second_position(char *image, char *name, int *second_offset, int *second_size)
{
	device_io_t device;
	stream_FILE* file;
	stream_VOLUME *volume;
	struct stream_stat st;

	device_sector_size = 2048;
	device.data = (void*)device_open(image, O_RDONLY);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;

	volume = iso9660_mount(&device);
	if (volume == NULL)
	{
		fprintf(stderr, "ERROR: Cannot open \"%s\"\n", image);
		return 1;
	}

	file = iso9660_open(volume, name);
	if (file == NULL)
	{
		fprintf(stderr, "ERROR: Cannot locate \"%s\" in the image\n", name);
		return 2;
	}

	iso9660_fstat(file, &st);

	*second_offset = st.st_base * 4;
	*second_size = st.st_size;

	iso9660_close(file);
	iso9660_umount(volume);
	device_close(device.data);

	return 0;
}

static int set_second(char *image, int second_offset,
		      char *kernel_image, char *cmdline, char *ramdisk)
{
	int fd;
	int ret;

	fd = open(image, O_RDWR);
	lseek(fd, second_offset * 512, SEEK_SET);
	ret = emile_second_set_param(fd, kernel_image, cmdline, ramdisk);
	close(fd);

	return ret;
}

static int set_config(char *image, int second_offset, char *config)
{
	int fd;
	int ret;
	char c[512];

	sprintf(c, "configuration %s\n", config);

	fd = open(image, O_RDWR);
	if (fd == -1)
		return -1;
	ret = lseek(fd, second_offset * 512, SEEK_SET);
	if (ret == -1) {
		close(fd);
		return -1;
	}

	emile_second_set_configuration(fd, (int8_t*)c);
	close(fd);

	return 0;
}

static int set_first(char *image, int drive_num, int second_offset, int second_size)
{
	int fd;
	map_t* map;
	int start, count;
	int ret;
	int i;
	int boottype;
	char bootblock[BOOTBLOCK_SIZE];
	device_io_t device;

	device_sector_size = 2048;
	device.data = (void*)device_open(image, O_RDONLY);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;

	map = map_open(&device);
	for (i = 0; i < map_get_number(map); i++)
	{
		map_read(map, i);
		if ( strcmp(APPLE_HFS, map_get_partition_type(map)) == 0) {
			ret = map_get_partition_geometry(map, &start, &count);
			map_bootblock_read(map, bootblock);
			boottype = map_bootblock_get_type(bootblock);
			if (boottype == EMILE_BOOTBLOCK)
			{
				printf("Bootable HFS partition found at position %d (%d * %d)\n",
						i, start, count);
				break;
			}
		}
	}
	map_close(map);

	if (i == map_get_number(map))
		return -1;

	fd = open(image, O_RDWR);
	ret = lseek(fd, start * 512, SEEK_SET);
	ret = emile_first_set_param_scsi_extents(fd, 3, second_offset / 4, second_size, BLOCKSIZE);
	close(fd);

	return ret;
}

int main(int argc, char** argv)
{
	int verbose = 0;
	int option_index = 0;
	char* first_level = NULL;
	char* second_level = NULL;
	char* kernel_image = NULL;
	char* ramdisk = NULL;
	char* image = NULL;
	char* appledriver = NULL;
	char* emiledriver = NULL;
	char *cmdline = NULL;
	char *conffile = NULL;
	int c;
	char temp[256];
	int i;
	char second_on_iso[256];
	int second_offset;
	int second_size;
	char *buffer;

	while(1)
	{
		c = getopt_long(argc, argv, "hvf:s:k:r:d:i:a:e:c:",
				long_options, &option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_VERBOSE:
			verbose = 1;
			break;
		case ARG_FIRST:
			first_level = optarg;
			break;
		case ARG_SECOND:
			second_level = optarg;
			break;
		case ARG_KERNEL:
			kernel_image = optarg;
			break;
		case ARG_RAMDISK:
			ramdisk = optarg;
			break;
		case ARG_APPEND:
			cmdline = optarg;
			break;
		case ARG_APPLEDRIVER:
			appledriver = optarg;
			break;
		case ARG_EMILEDRIVER:
			emiledriver = optarg;
			break;
		case ARG_CONFIG:
			conffile = optarg;
			break;
		}
	}

	if (optind < argc)
		image = argv[optind++];

	if (image == NULL)
	{
		fprintf(stderr,"ERROR: you must provide a filename to write image\n");
		usage(argc, argv);
		return 1;
	}

	if (appledriver == NULL && emiledriver == NULL)
		emiledriver = DRIVER_PATH;

	if (conffile && (kernel_image || ramdisk || cmdline)) {
		fprintf(stderr, "ERROR: --config cannot be used with --kernel,"
		        " --ramdisk or --append\n");
		usage(argc, argv);
		return 1;
	}
        if (emiledriver == NULL) {
		if (first_level == NULL)
			first_level = FIRST_PATH;

		if (second_level == NULL)
			second_level = SECOND_PATH;

		if (create_apple_driver(temp, appledriver, first_level))
			return 1;
	} else { 
		char name[32];
		int fd_in, fd_out;
		char buf[512];
		int ret;

		if (first_level != NULL ||
		    second_level != NULL ||
		    appledriver != NULL) {
			fprintf(stderr,
			        "ERROR: --emiledriver cannot be used with "
				"--appledriver, --first or -second\n");
		        usage(argc, argv);
			return 1;
		}

		fd_in = open(emiledriver, O_RDONLY);
		if (fd_in == -1)
		{
			fprintf(stderr, "ERROR: cannot open %s\n", emiledriver);
			return 1;
		}

		sprintf(name, "/tmp/emile_driver-XXXXXX");
		mktemp(name);
		fd_out = open(name, O_RDWR | O_CREAT, S_IRWXU);
		if (fd_out == -1)
		{
			fprintf(stderr, "ERROR: cannot open %s (%s)\n", name, strerror(errno));
			return 1;
		}

		while ( (ret = read(fd_in, buf, 512)) > 0 )
		{
			ret = write(fd_out, buf, ret);
			if (ret == -1)
				break;
		}
		close(fd_in);
		close(fd_out);
		if (ret == -1)
		{
			fprintf(stderr, "ERROR: cannot copy %s to %s\n", emiledriver, name);
			return 1;
		}

		if (conffile)
			set_config(name, 0, conffile);
		else
			set_second(name, 0, kernel_image, cmdline, ramdisk);

		if (create_apple_driver(temp, name, first_level))
			return 1;

		unlink(name);
	}

	buffer = malloc(65536);

	if (second_level) {
		strcpy(second_on_iso, kernel_image);
		dirname(second_on_iso);
		strcat(second_on_iso, "/emile");
		if (verbose)
			sprintf(buffer, COMMAND " %s=%s", "",
				temp, image, second_on_iso, second_level);
		else
			sprintf(buffer, COMMAND " %s=%s", "-quiet",
				temp, image, second_on_iso, second_level);
	} else {
		if (verbose)
			sprintf(buffer, COMMAND, "", temp, image);
		else
			sprintf(buffer, COMMAND, "-quiet", temp, image);
	}

	for (i = optind; i < argc; i++)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[i]);
	}
	printf("%s\n",buffer);
	system(buffer);

	free(buffer);

	unlink(temp);

	if (second_level) {
		if (get_second_position(image, second_on_iso, 
					&second_offset, &second_size))
		{
			return 2;
		}

		printf("Second is at %d * %d\n", second_offset, second_size);

		set_first(image, 3, second_offset, second_size);

		if (conffile)
			set_config(emiledriver, second_offset, conffile);
		else
			set_second(image, second_offset,
				   kernel_image, cmdline, ramdisk);
	}

	return 0;
}
