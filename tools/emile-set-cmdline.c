/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "emile.h"
#include "emile-first.h"
#include "emile-second.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s <image> <cmdline>\n", argv[0]);
	fprintf(stderr, "Usage: %s -r <image>\n", argv[0]);
	fprintf(stderr, "\n     Allows to set the kernel command line <cmdline>\n");
	fprintf(stderr, "     into the floppy image <image>\n");
	fprintf(stderr, "     <image> can be a file or a device (/dev/fd0)\n");
	fprintf(stderr, "     with \"-r\" flag, display current command line\n");
	fprintf(stderr, "\n     Examples:\n");
	fprintf(stderr, "\n     To set root filesystem on disk 1 partition 4\n");
	fprintf(stderr, "\n     %s floppy.img \"root=/dev/sda4\"\n", argv[0]);
	fprintf(stderr, "\n     To set root filesystem on ramdisk\n");
	fprintf(stderr, "\n     %s floppy.img \"root=/dev/ramdisk ramdisk_size=2048\"\n", argv[0]);
	fprintf(stderr, "\n     To set root filesystem on NFS\n");
	fprintf(stderr, "\n     %s floppy.img \"root=/dev/nfs ip=dhcp nfsroot=192.168.100.1:/tftboot/192.168.100.51/\"\n", argv[0]);
	/* and when kernel will support floppy driver:
         * KERNEL_ARGS="vga=normal noinitrd load_ramdisk=1 prompt_ramdisk=1 ramdisk_size=16384 root=/dev/fd0 disksize=1.44 flavor=compact"
	 */
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int set_cmdline(int readonly, char* image, char* cmdline)
{
	emile_l2_header_t header;
	int fd;
	int ret;

	if (readonly)
		fd = open(image, O_RDONLY);
	else
		fd = open(image, O_RDWR);

	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
	if (ret == -1)
	{
		perror("Cannot go to buffer offset");
		close(fd);
		return 3;
	}

	ret = read(fd, &header, sizeof(header));
	if (ret != sizeof(header))
	{
		perror("Cannot read current command line");
		close(fd);
		return 7;
	}

	if (EMILE_001_SIGNATURE != read_long(&header.signature))
	{
		fprintf(stderr, "Bad Header signature\n");
		return 8;
	}

	if (readonly)
		printf("Current command line: \"%s\"\n", header.command_line);
	else
	{
		strncpy(header.command_line, cmdline, 256);
		header.command_line[255] = 0;

		ret = lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
		if (ret == -1)
		{
			perror("Cannot go to buffer offset");
			close(fd);
			return 3;
		}

		ret = write(fd, &header, sizeof(header));
		if (ret != sizeof(header))
		{
			perror("Cannot set command line");
			close(fd);
			return 5;
		}

		printf("Command line successfully modified\n");
	}

	close(fd);

	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	if (argc != 3)
	{
		usage(argc, argv);
		return 1;
	}

	if (strcmp(argv[1], "-r") == 0)
		ret = set_cmdline(1, argv[2], NULL);
	else
		ret = set_cmdline(0, argv[1], argv[2]);

	return ret;
}
