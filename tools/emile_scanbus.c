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

int emile_scanbus(device_name_t devices[EMILE_MAX_DISK])
{
	int i,j;
	int fd;
	device_name_t	dev;

	j = 0;
	for(i = 0; i < EMILE_MAX_DISK; i++)
	{
		sprintf(dev, "/dev/sd%c", 'a' + i);
		fd = open(dev, O_RDONLY);
		if (fd == -1)
		{
			if (errno == ENXIO)
				continue;
			return j;
		}
		close(fd);
		strncpy(devices[j++], dev, EMILE_MAX_DEVNAME);
	}
	return j;
}

int seek_partition(char *dev, int base)
{
	emile_map_t* map;
	int i;
	int start;
	int count;

	map = emile_map_open(dev, O_RDONLY);
	for (i = 0; i < emile_map_get_number(map); i++)
	{
		emile_map_read(map, i);
		emile_map_get_partition_geometry(map, &start, &count);
		if (base == start)
		{
			emile_map_close(map);
			return i;
		}
	}
	emile_map_close(map);
	return -1;
}

void scanbus(void)
{
	emile_map_t* map;
	device_name_t devices[EMILE_MAX_DISK];
	int count;
	int i;
	int j;
	char bootblock[BOOTBLOCK_SIZE];

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
	{
		int block_size, block_count;

		printf("%s:", devices[i]);
		map = emile_map_open(devices[i], O_RDONLY);
		if (verbose)
		{
			emile_map_geometry(map, &block_size, &block_count);
			printf(" block size: %d, blocks number: %d (", 
				block_size, block_count);
			print_size(block_count, block_size);
			printf(")\n");
		}
		else putchar('\n');

		if (map == NULL)
		{
			printf("\t<No information available>\n");
			continue;
		}
		if (!emile_map_is_valid(map))
		{
			printf("\t<No valid partition map found>\n");
			continue;
		}
		if (emile_map_get_driver_number(map) > 0)
			printf("  Drivers\n");
		for (j = 0; j < emile_map_get_driver_number(map); j++)
		{
			int block, size, type, part;
			emile_map_get_driver_info(map, j, 
						  &block, &size, &type);
			printf("     %d: base: %d size: %d type: %d",
			       j, block, size, type);
			part = seek_partition(devices[i], block);
			if (part == -1)
				printf(" <invalid>\n");
			else
			{
				emile_map_read(map, part);
				printf(" <%d: %s [%s]>\n", part + 1,
					emile_map_get_partition_name(map),
					emile_map_get_partition_type(map));
			}
		}
		printf("  Partitions\n");
		for (j = 0; j < emile_map_get_number(map); j++)
		{
			emile_map_read(map, j);

			if (emile_map_partition_is_startup(map))
				printf(" --> ");
			else
				printf("     ");
			printf("%s%-2d: ", devices[i], j + 1);
			printf("%16s [%-16s] ", 
				emile_map_get_partition_name(map),
				emile_map_get_partition_type(map));
			if (emile_map_partition_is_bootable(map))
			{
				int boottype;
				emile_map_bootblock_read(map, bootblock);
				boottype = emile_map_bootblock_get_type(bootblock);
				switch(boottype)
				{
				case INVALID_BOOTBLOCK:
					printf(" <no bootblock>\n");
					break;
				case APPLE_BOOTBLOCK:
					printf(" <Apple bootblock>\n");
					break;
				case EMILE_BOOTBLOCK:
					printf(" <EMILE bootblock>\n");
					break;
				default:
					printf(" <unknown bootblock>\n");
					break;
				}
			}
			else
				putchar('\n');
			if (verbose)
			{
				int start, count;

				emile_map_get_partition_geometry(map,
								&start, &count);
				printf("                 base: %d, count: %d (", 
					start, count);
				print_size(count, block_size);
				printf(")\n");
				printf("                 flags: 0x%08x\n", 
					emile_map_partition_get_flags(map));
			}
		}
		emile_map_close(map);
	}
}
