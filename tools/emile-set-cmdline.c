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

#include "emile-first.h"

#define CMDLINE_OFFSET		(FIRST_LEVEL_SIZE + 4)

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
}

int set_cmdline(int readonly, char* image, char* cmdline)
{
	int fd;
	short buffer_size;
	int ret;
	int len;

	if (readonly)
		fd = open(image, O_RDONLY);
	else
		fd = open(image, O_RDWR);

	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = lseek(fd, CMDLINE_OFFSET, SEEK_SET);
	if (ret == -1)
	{
		perror("Cannot go to buffer offset");
		close(fd);
		return 3;
	}

	ret = read(fd, &buffer_size, sizeof(buffer_size));
	if (ret != sizeof(buffer_size))
	{
		perror("Cannot read buffer size");
		close(fd);
		return 3;
	}

	if (readonly)
	{
		char* buffer;

		buffer = (char*)malloc(buffer_size);
		if (buffer == NULL)
		{
			perror("Cannot malloc()");
			close(fd);
			return 6;
		}

		ret = read(fd, buffer, buffer_size);
		if (ret != buffer_size)
		{
			perror("Cannot read current command line");
			free(buffer);
			close(fd);
			return 7;
		}
		printf("Current command line: \"%s\"\n", buffer);
		free(buffer);
	}
	else
	{
		len = strlen(cmdline) + 1;
		if (len > buffer_size)
		{
			fprintf(stderr, "Command line too long\n");
			close(fd);
			return 4;
		}

		ret = write(fd, cmdline, len);
		if (ret != len)
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
