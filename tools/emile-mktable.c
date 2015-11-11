/*
 *
 * (c) 2015 Laurent Vivier <Laurent@Vivier.EU>
 *
 * HOWTO create a bootable disk: 
 *
 *     emile-mktable -e second/m68k-linux-scsi-driver/apple_driver /dev/sdh
 *
 */

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
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/mount.h>

#include <libmap.h>
#include <emile.h>
#include <libemile.h>
#include <libiso9660.h>
#include <libstream.h>

#include "device.h"

#define DRIVER_SIZE	(64*1024)

#define DRIVER_PATH	"/lib/emile/apple_driver"

#define PARTITION_TABLE_LEN	63
#define DRIVER_BASE		(PARTITION_TABLE_LEN + 1)
#define MAP_BLOCK_COUNT		4	/* 4 entries in the table: map, driver, HFS, extra */
#define EXTRA_BASE		(4*1024*1024/512) /* 4 MB */

enum {
	ARG_NONE = 0,
	ARG_HELP ='h',
	ARG_EMILEDRIVER = 'e',
	ARG_FORCE_SIZE = 'f',
};

static struct option long_options[] = 
{
	{"help",	0, NULL,	ARG_HELP	},
	{"emiledriver",	1, NULL,	ARG_EMILEDRIVER },
	{"force-size",  1, NULL,	ARG_FORCE_SIZE  },
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage %s [FLAGS] disk\n", 
			argv[0]);
	fprintf(stderr, "Create and EMILE bootable disk\n");
	fprintf(stderr, "   -h, --help               display this text\n");
	fprintf(stderr, "   -f, --force-size=SECTORS "
			"force the size of the disk in the table\n");
	fprintf(stderr, "   -e, --emiledriver=FILE   "
	                "emiledriver to copy to disk\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

#define BLOCKSIZE	(512)

static int emile_mktable(char *filename, char *appledriver, unsigned long force_size)
{
	struct DriverDescriptor block0;
	struct Partition *map512;
	int fd_driver;
	unsigned char *driver;
	struct stat st;
	int driver_size;
	int fd;
	int i;
	unsigned long disk_block_count;
	u_int32_t driver_block_count, extra_block_count, hfs_block_count;
	int current;

	/* open disk */

	fd = open(filename, O_WRONLY);
	if (fd == -1) {
		fprintf(stderr,
			"ERROR: Cannot open file %s\n",
			filename);
		return -1;
	}
	if (force_size == 0) {
		if (ioctl(fd, BLKGETSIZE, &disk_block_count) == -1) {
			fstat(fd, &st);
			disk_block_count = (st.st_size + 512 - 1) / 512;
		}
	} else {
		disk_block_count = force_size;
	}

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

	/*	Driver Descriptor				512 bytes
	 *      Driver Partition Map (for 512 byte blocks)      512 bytes
	 *      Driver Partition                                N x 512 bytes
	 *      HFS
	 *      Extra
	 */


	/* initialize block 0 with the size of the disk and the number of driver */

	memset(&block0, 0, sizeof(block0));
	write_short((u_int16_t*)&block0.Sig, DD_SIGNATURE);
	write_short((u_int16_t*)&block0.BlkSize, BLOCKSIZE);
	write_long((u_int32_t*)&block0.BlkCount, disk_block_count);
	write_short((u_int16_t*)&block0.DevType, 1);
	write_short((u_int16_t*)&block0.DevId, 1);
	write_long((u_int32_t*)&block0.Data, 0);
	write_short((u_int16_t*)&block0.DrvrCount, 1);

 	/* initialize driver info */

	write_long((u_int32_t*)&block0.DrvInfo[0].Block, DRIVER_BASE);
	write_short((u_int16_t*)&block0.DrvInfo[0].Size, (driver_size + BLOCKSIZE - 1) / BLOCKSIZE);
	write_short((u_int16_t*)&block0.DrvInfo[0].Type, kDriverTypeMacSCSI);

	/* initialize partition table */

	map512 = malloc(PARTITION_TABLE_LEN * 512);
	if (map512 == NULL)
	{ 
		fprintf(stderr, "Cannot malloc %d bytes\n",
			PARTITION_TABLE_LEN * 512);
		return -1;
	}

	memset(map512, 0, PARTITION_TABLE_LEN * 512);
	current = 0;

	/*            partition table entry */

	write_short((u_int16_t*)&map512[current].Sig, MAP_SIGNATURE);
	write_long((u_int32_t*)&map512[current].MapBlkCnt, MAP_BLOCK_COUNT);
	write_long((u_int32_t*)&map512[current].PyPartStart, 1);
	write_long((u_int32_t*)&map512[current].PartBlkCnt, PARTITION_TABLE_LEN);
	strncpy(map512[current].PartName, "Apple", 32);
	strncpy(map512[current].PartType, APPLE_PARTITION_MAP, 32);
	write_long((u_int32_t*)&map512[current].DataCnt, PARTITION_TABLE_LEN);
	write_long((u_int32_t*)&map512[current].PartStatus,
                   kPartitionAUXIsWriteable | kPartitionAUXIsReadable |
		   kPartitionAUXIsInUse | kPartitionAUXIsAllocated |
		   kPartitionAUXIsValid );
	current++;

	/* initialize driver partition entry */

	driver_block_count = (driver_size + 512 - 1) / 512;
	write_short((u_int16_t*)&map512[current].Sig, MAP_SIGNATURE);
	write_long((u_int32_t*)&map512[current].MapBlkCnt, MAP_BLOCK_COUNT);
	write_long((u_int32_t*)&map512[current].PartBlkCnt, driver_block_count);
	write_long((u_int32_t*)&map512[current].PyPartStart, DRIVER_BASE);
	strncpy(map512[current].PartName, "Macintosh", 32);
	strncpy(map512[current].PartType, APPLE_DRIVER_EMILE, 32);
	write_long((u_int32_t*)&map512[current].DataCnt, driver_block_count);
	write_long((u_int32_t*)&map512[current].PartStatus, kPartitionAUXIsValid | 
				   kPartitionAUXIsAllocated | 
				   kPartitionAUXIsInUse | 
				   kPartitionAUXIsBootValid | 
				   kPartitionAUXIsReadable | 
				   kPartitionAUXIsWriteable | 
				   kPartitionAUXIsBootCodePositionIndependent | 
				   kPartitionIsChainCompatible | 
				   kPartitionIsRealDeviceDriver);
	 write_long((u_int32_t*)&map512[current].BootSize, st.st_size);
	write_long((u_int32_t*)&map512[current].BootCksum, map_checksum(driver, st.st_size));
	strncpy(map512[current].Processor, "68000", 16);
	write_long((u_int32_t*)&map512[current].Pad, kSCSIDriverSignature);
	current++;

	/*            HFS table entry */

	hfs_block_count = EXTRA_BASE - (DRIVER_BASE + driver_block_count);
	write_short((u_int16_t*)&map512[current].Sig, MAP_SIGNATURE);
	write_long((u_int32_t*)&map512[current].MapBlkCnt, MAP_BLOCK_COUNT);
	write_long((u_int32_t*)&map512[current].PyPartStart, DRIVER_BASE + driver_block_count);
	write_long((u_int32_t*)&map512[current].PartBlkCnt, hfs_block_count);
	strncpy(map512[current].PartName, "MacOS", 32);
	strncpy(map512[current].PartType, APPLE_HFS, 32);
	write_long((u_int32_t*)&map512[current].DataCnt, hfs_block_count);
	current++;

	/*            extra (free) table entry */

	extra_block_count = disk_block_count - EXTRA_BASE;
	write_short((u_int16_t*)&map512[current].Sig, MAP_SIGNATURE);
	write_long((u_int32_t*)&map512[current].MapBlkCnt, MAP_BLOCK_COUNT);
	write_long((u_int32_t*)&map512[current].PyPartStart, EXTRA_BASE);
	write_long((u_int32_t*)&map512[current].PartBlkCnt, extra_block_count);
	strncpy(map512[current].PartName, "Extra", 32);
	strncpy(map512[current].PartType, APPLE_FREE, 32);
	write_long((u_int32_t*)&map512[current].DataCnt, extra_block_count);
	current++;

	/* write block 0*/

	write(fd, &block0, sizeof(block0));

	/* write partition table */

	for (i = 0; i < PARTITION_TABLE_LEN; i++)
	{
		write(fd, map512 + i, 512);
	}

	/* write driver */

	write(fd, driver, driver_size);
	free(driver);

	close(fd);

	return 0;
}

int main(int argc, char** argv)
{
	int option_index = 0;
	char* image = NULL;
	char* emiledriver = NULL;
	unsigned long force_size = 0;
	int c;

	while(1)
	{
		c = getopt_long(argc, argv, "he:f:",
				long_options, &option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_EMILEDRIVER:
			emiledriver = optarg;
			break;
		case ARG_FORCE_SIZE:
			force_size = strtoull(optarg, NULL, 0);
			if (force_size == ULONG_MAX && errno == ERANGE) {
				fprintf(stderr, "ERROR: invalid size\n");
				return 1;
			}
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

	if (emiledriver == NULL)
		emiledriver = DRIVER_PATH;


	if (emile_mktable(image, emiledriver, force_size))
		return 1;

	return 0;
}
