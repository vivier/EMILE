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

#include "libemile.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s <file> <cmdline>\n", argv[0]);
	fprintf(stderr, "Usage: %s -r <file>\n", argv[0]);
	fprintf(stderr, "\n     Allows to set the kernel command line <cmdline>\n");
	fprintf(stderr, "     into the floppy image or the second level file\n");
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

int set_cmdline(char* image, char* cmdline)
{
	int fd;
	int ret;

	fd = open(image, O_RDWR);

	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_second_set_param(fd, NULL, cmdline, NULL);

	close(fd);

	return 0;
}

int get_cmdline(char* image)
{
	int fd;
	int ret;
	char cmdline[255];

	fd = open(image, O_RDONLY);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = emile_second_get_param(fd, NULL, cmdline, NULL);

	if (cmdline[0])
		printf("Current command line: \"%s\"\n", cmdline);
	else
		fprintf(stderr, "No command line found\n");

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
		ret = get_cmdline(argv[2]);
	else
		ret = set_cmdline(argv[1], argv[2]);

	return ret;
}
