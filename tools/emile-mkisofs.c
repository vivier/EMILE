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
//-f ../first/first_scsi -s ../second/m68k-linux-scsi/second -k "/install/kernels/vmlinuz-2.2.25-mac" -r "/install/cdrom/initrd22.gz" -d Apple_Driver43 -a "root=/dev/ramdisk ramdisk_size=13000" boot.img /mnt/cdrom

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <partition.h>
#include <emile.h>
#include <libemile.h>
#include <libiso9660.h>
#include <libstream.h>

#include "device.h"

#define COMMAND "/usr/bin/mkisofs %s -hfs -joliet -R -boot-hfs-file %s -graft-points -o %s %s=%s"

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
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage %s [FLAGS] filename pathspec [pathspec ...]\n", 
			argv[0]);
	fprintf(stderr, "Create and EMILE bootable CDROM\n");
	fprintf(stderr, "   -h, --help        display this text\n");
	fprintf(stderr, "   -v, --verbose     verbose mode\n");
	fprintf(stderr, "   -f, --first       first level to copy to CDROM\n");
	fprintf(stderr, "   -s, --second      second level to copy to CDROM\n");
	fprintf(stderr, "   -k, --kernel      path of the kernel on CDROM\n");
	fprintf(stderr, "   -r, --ramdisk     path of ramdisk on CDROM\n");
	fprintf(stderr, "   -a, --append      set kernel command line\n");
	fprintf(stderr, "   -d, --appledriver appledriver to copy to CDROM\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int create_apple_driver(char *temp, char *appledriver, char *first_level)
{
	struct DriverDescriptor block0;
	struct Partition map2048;
	struct Partition map512;
	FILE* fd;
	FILE* fd_driver;
	char *buffer;
	int ret;
	int i;

	/*	HFS CD Label Block                              512 bytes
	 *      Driver Partition Map (for 2048 byte blocks)     512 bytes
	 *      Driver Partition Map (for 512 byte blocks)      512 bytes
	 *      Empty                                           512 bytes
	 *      Driver Partition                                N x 2048 bytes
	 *      HFS Partition Boot Block                        1024 bytes
	 */


	memset(&block0, 0, sizeof(block0));
	write_short(&block0.Sig, DD_SIGNATURE);
	write_short(&block0.BlkSize, 2048);
	write_long(&block0.BlkCount, 0);
	write_short(&block0.DevType, 1);
	write_short(&block0.DevId, 1);
	write_long(&block0.Data, 0);
	write_short(&block0.DrvrCount, 2);

	write_long(&block0.DrvInfo[0].Block, 16);	/* start block in BlkSize blocks */
	write_short(&block0.DrvInfo[0].Size, 4);	/* in 512 block size */
	write_short(&block0.DrvInfo[0].Type, kDriverTypeMacSCSI);

	write_long(&block0.DrvInfo[1].Block, 48);	/* start block in BlkSize blocks */
	write_short(&block0.DrvInfo[1].Size, 20);	/* in 512 block size */
	write_short(&block0.DrvInfo[1].Type, kDriverTypeMacATA);

	memset(&map512, 0, sizeof(map512));
	write_short(&map512.Sig, MAP_SIGNATURE);
	write_long(&map512.MapBlkCnt, 5);
	write_long(&map512.PyPartStart, 64);	// 64 * 512
	write_long(&map512.PartBlkCnt, 128);	// 128 * 512
	strncpy(map512.PartName, "Macintosh", 32);
	strncpy(map512.PartType, APPLE_DRIVER43, 32);
	write_long((unsigned int*)(map512.PartType + 28), kPatchSnag);
	write_long(&map512.LgDataStart, 0);
	write_long(&map512.DataCnt, 128);
	write_long(&map512.PartStatus, 0x0000007f);
	write_long(&map512.LgBootStart, 0);
	write_long(&map512.BootSize, 9622);
	write_long(&map512.BootAddr, 0);
	write_long(&map512.BootAddr2, 0);
	write_long(&map512.BootEntry, 0);
	write_long(&map512.BootEntry2, 0);
	write_long(&map512.BootCksum, 0xb7fb);
	strncpy(map512.Processor, "68000", 16);
	map512.Pad[1] = 0x5200;
	map512.Pad[3] = 0x0e00;
	map512.Pad[13] = 0x2b00;

	map2048 = map512;
	write_long(&map2048.BootSize, 7146);
	write_long(&map2048.PyPartStart, 16);
	write_long(&map2048.PartBlkCnt, 32);
	write_long(&map2048.BootCksum, 0x84b8);

	write_long((unsigned int*)(map512.Pad + 186), 0x4d524b53); // 'MRKS'

	strcpy(temp, "/tmp/emile-mkisofs-XXXXXX");
	mkstemp(temp);
	fd = fopen(temp, "w");

	ret = fwrite(&block0, 1, sizeof(block0), fd);
	ret = fwrite(&map2048, 1, sizeof(map2048), fd);
	ret = fwrite(&map512, 1, sizeof(map512), fd);
	memset(&map512, 0, sizeof(map512));
	ret = fwrite(&map512, 1, sizeof(map512), fd);

	buffer = malloc(read_short(&block0.BlkSize));
	fd_driver = fopen(appledriver, "r");
	for(i = 0; i < read_long(&map2048.PartBlkCnt); i++)
	{
		memset(buffer, 0, read_short(&block0.BlkSize));
		ret = fread(buffer, 1, read_short(&block0.BlkSize), fd_driver);
		fwrite(buffer, read_short(&block0.BlkSize), 1, fd);
	}
	free(buffer);
	fclose(fd_driver);

	buffer = malloc(1024);
	fd_driver = fopen(first_level, "r");
	fread(buffer, 1, 1024, fd_driver);
	fwrite(buffer, 1024, 1, fd);
	fclose(fd_driver);
	free(buffer);

	fclose(fd);

	return 0;
}

static int get_second_position(char *image, char *name, int *second_offset, int *second_size)
{
	device_io_t device;
	iso9660_FILE* file;
	iso9660_VOLUME *volume;

	device.data = device_open(image);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;

	volume = iso9660_mount(&device);
	if (volume == NULL)
		return 1;

	file = iso9660_open(volume, name);

	*second_offset = file->base * 4;
	*second_size = file->size;

	iso9660_close(file);
	iso9660_umount(volume);
	device_close(device.data);

	return 0;
}

static int set_second(char *image, int second_offset, char *kernel_image, char *cmdline, char *ramdisk)
{
	int fd;
	int ret;
	char k[512], r[512];

	sprintf(k, "iso9660:(sd3)%s", kernel_image);
	sprintf(r, "iso9660:(sd3)%s", ramdisk);

	fd = open(image, O_RDWR);
	lseek(fd, second_offset * 512, SEEK_SET);
	ret = emile_second_set_param(fd, k, cmdline, r);
	close(fd);

	return ret;
}

static int set_first(char *image, int drive_num, int second_offset, int second_size)
{
	int fd;
	emile_map_t* map;
	int start, count;
	int ret;
	int i;
	int boottype;
	char bootblock[BOOTBLOCK_SIZE];

	map = emile_map_open(image, O_RDONLY);
	for (i = 0; i < emile_map_get_number(map); i++)
	{
		emile_map_read(map, i);
		if ( strcmp(APPLE_HFS, emile_map_get_partition_type(map)) == 0) {
			ret = emile_map_get_partition_geometry(map, &start, &count);
			emile_map_bootblock_read(map, bootblock);
			boottype = emile_map_bootblock_get_type(bootblock);
			if (boottype == EMILE_BOOTBLOCK)
			{
				printf("Bootable HFS partition found at position %d (%d * %d)\n",
						i, start, count);
				break;
			}
		}
	}
	emile_map_close(map);

	if (i == emile_map_get_number(map))
		return -1;

	fd = open(image, O_RDWR);
	ret = lseek(fd, start * 512, SEEK_SET);
	ret = emile_first_set_param_scsi_extents(fd, 3, second_offset / 4, second_size);
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
	char *cmdline = NULL;
	int c;
	char temp[256];
	int i;
	char second_on_iso[256];
	int second_offset;
	int second_size;
	char *buffer;


	while(1)
	{
		c = getopt_long(argc, argv, "hvf:s:k:r:d:i:a:", long_options,
				&option_index);
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

	create_apple_driver(temp, appledriver, first_level);

	buffer = malloc(65536);
	
	strcpy(second_on_iso, kernel_image);
	dirname(second_on_iso);
	strcat(second_on_iso, "/emile");

	if (verbose)
		sprintf(buffer, COMMAND, "", temp, image, second_on_iso, second_level);
	else
		sprintf(buffer, COMMAND, "-quiet", temp, image, second_on_iso, second_level);

	for (i = optind; i < argc; i++)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[i]);
	}
	printf("%s\n",buffer);
	system(buffer);

	free(buffer);

	unlink(temp);

	get_second_position(image, second_on_iso, &second_offset, &second_size);

	printf("Second is at %d * %d\n", second_offset, second_size);

	set_first(image, 3, second_offset, second_size);

	set_second(image, second_offset, kernel_image, cmdline, ramdisk);

	return 0;
}
