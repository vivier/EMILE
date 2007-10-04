/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
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

#include "libmap.h"
#include "libemile.h"

#define EMILE_MAX_DISK		16
#define EMILE_MAX_DEVNAME	16

typedef char device_name_t[EMILE_MAX_DEVNAME];

extern int verbose;

static void print_size(int nb_blocks, int block_size)
{
	int B, GB, MB, kB;

	B = (nb_blocks * block_size) % 1024;

	if (block_size > 1024)
		kB = nb_blocks * (block_size / 1024);
	else
		kB = nb_blocks / (1024 / block_size);
	
	MB = kB / 1024;
	kB = kB % 1024;

	GB = MB / 1024;
	MB = MB % 1024;

	if (GB)
		printf("%d.%03d GB", GB, MB * 1000 / 1024);
	else if (MB)
		printf("%d.%03d MB", MB, kB * 1000 / 1024);
	else
		printf("%d.%03d kB", kB, B * 1000 / 1024);
}

static int emile_scanbus(device_name_t devices[EMILE_MAX_DISK])
{
	int i,j;
	int fd;
	device_name_t	dev;

	j = 0;

	/* scan SCSI disks */

	for(i = 0; (i < EMILE_MAX_DISK) && (j < EMILE_MAX_DISK); i++)
	{
		sprintf(dev, "/dev/sd%c", 'a' + i);
		fd = open(dev, O_RDONLY);
		if (fd == -1)
			break;
		close(fd);
		strncpy(devices[j++], dev, EMILE_MAX_DEVNAME);
	}

	/* scan ATA disks: EMILE doesn't manage them, but this
	 * allows to have all information on all disks
	 */

	for(i = 0; (i < EMILE_MAX_DISK) && (j < EMILE_MAX_DISK); i++)
	{
		sprintf(dev, "/dev/hd%c", 'a' + i);
		fd = open(dev, O_RDONLY);
		if (fd == -1)
			continue;
		close(fd);
		strncpy(devices[j++], dev, EMILE_MAX_DEVNAME);
	}

	return j;
}

void diskinfo(char* device)
{
	map_t* map;
	int j;
	int boottype;
	char bootblock[BOOTBLOCK_SIZE];
	int block_size, block_count;
	int ret;

	printf("%s:", device);
	map = map_open(device, O_RDONLY);
	if (map == NULL)
	{
		fprintf(stderr, "Cannot read map of %s\n", device);
		return;
	}

	ret = map_geometry(map, &block_size, &block_count);
	if ((ret != -1) && verbose)
	{
		printf(" block size: %d, blocks number: %d (", 
			block_size, block_count);
		print_size(block_count, block_size);
		printf(")\n");
	}
	else putchar('\n');

	if (map == NULL)
	{
		printf("\t<No information available>\n");
		return;
	}
	if (!map_is_valid(map))
	{
		printf("\t<No valid partition map found>\n");
		return;
	}
	if (map_get_driver_number(map) > 0)
		printf("  Drivers\n");
	for (j = 0; j < map_get_driver_number(map); j++)
	{
		int block, size, type, part;
		map_get_driver_info(map, j, 
					  &block, &size, &type);
		printf("     %d: base: %d size: %d ",
		       j, block * block_size / 512, 
		       size * block_size / 512);
		printf("type: ");
		switch(type)
		{
			case kDriverTypeMacSCSI:
				printf("SCSI");
				break;
			case kDriverTypeMacATA:
				printf("ATA");
				break;
			case kDriverTypeMacSCSIChained:
				printf("SCSI chained");
				break;
			case kDriverTypeMacATAChained:
				printf("ATA chained");
				break;
			default:
				printf("unknown (%04x)\n", type);
				break;
		}
		part = map_seek_driver_partition(map, 
					block * block_size / 512);
		if (part == -1)
		{
			part = map_seek_driver_partition(map, 
							       block);
			if (part == -1)
				printf(" <invalid>\n");
			map_read(map, part);
		}
		else
		{
			map_read(map, part);
			if (!emile_is_apple_driver(map))
			{
				part = map_seek_driver_partition(map, 
								       block);
				if (part == -1)
					printf(" <invalid>\n");
				map_read(map, part);
			}
		}
		printf(" <%d: %s [%s], ", part + 1,
			map_get_partition_name(map),
			map_get_partition_type(map));
		switch(map_get_driver_signature(map))
		{
			case kPatchDriverSignature:
				printf("patch driver");
				break;
			case kSCSIDriverSignature:
				printf("SCSI HD driver");
				break;
			case kATADriverSignature:
				printf("ATAPI HD driver");
				break;
			case kSCSICDDriverSignature:
				printf("SCSI CDROM driver");
				break;
			case kATAPIDriverSignature:
				printf("ATAPI CDROM driver");
				break;
			case kDriveSetupHFSSignature:
				printf("Drive Setup HFS partition");
				break;
			default:
				printf("Unknown (0x%08lx)", map_get_driver_signature(map));
				break;
		}
		printf(">\n");
	}
	printf("  Partitions\n");
	for (j = 0; j < map_get_number(map); j++)
	{
		map_read(map, j);

		if (map_partition_is_startup(map))
			printf(" --> ");
		else
			printf("     ");
		printf("%d: ", j + 1);
		printf("%16s [%-16s] ", 
			map_get_partition_name(map),
			map_get_partition_type(map));
		map_bootblock_read(map, bootblock);
		boottype = map_bootblock_get_type(bootblock);
		switch(boottype)
		{
		case INVALID_BOOTBLOCK:
			break;
		case APPLE_BOOTBLOCK:
			printf(" <Apple bootblock>");
			break;
		case EMILE_BOOTBLOCK:
			printf(" <EMILE bootblock>");
			break;
		default:
			printf(" <unknown bootblock>");
			break;
		}
		if (map_partition_is_bootable(map))
			printf(" *\n");
		else
			putchar('\n');
		if (verbose)
		{
			int start, count;
			int bootstart, bootsize, bootaddr, bootentry;
			int checksum;
			char processor[16];
			if (*(unsigned long*)
				(map_get_partition_type(map) + 28))
				printf("                 patch: %s\n", 
					map_get_partition_type(map) + 28);

			ret = map_get_partition_geometry(map,
							       &start,
							       &count);
			if( ret != -1)
			{
				printf("                 base:"
				       " %d, count: %d (", 
					start, count);
				print_size(count, 512);
				printf(")\n");
				printf("                 flags: 0x%08x\n", 
					map_partition_get_flags(map));
					map_get_bootinfo(map, &bootstart, 
					&bootsize, &bootaddr, 
					&bootentry, &checksum, 
					processor);
				printf("                 "
			       		"Bootstart: %d, Bootsize: %d\n",
			       		bootstart, bootsize);
				printf("                 "
			       		"Bootaddr: %d, Bootentry: %d\n", 
			       		bootaddr, bootentry);
				printf("                 "
			       		"Checksum: 0x%04x, Processor: %s\n", 
					checksum, processor);
			}
		}
	}
	map_close(map);
}

void scanbus(void)
{
	device_name_t devices[EMILE_MAX_DISK];
	int count;
	int i;

	count = emile_scanbus(devices);
	if (count == 0)
	{
		if (errno == EACCES)
		{
			fprintf(stderr, 
	"ERROR: cannot access to devices (you should try as root...)\n");
			return;
		}
		printf("No disk found\n");
	}
	for (i = 0; i < count; i++)
		diskinfo(devices[i]);
}
