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

#define SECTOR_SIZE		512
#define FIRST_LEVEL_SIZE	(SECTOR_SIZE * 2)
#define CMDLINE_OFFSET		(FIRST_LEVEL_SIZE + 4)

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s <image> <cmdline>\n", argv[0]);
	fprintf(stderr, "\n     Allows to set the kernel command line <cmdline>\n");
	fprintf(stderr, "     into the floppy image <image>\n");
	fprintf(stderr, "     <image> can be a file or a device (/dev/fd0)\n");
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

int set_cmdline(char* image, char* cmdline)
{
	int fd;
	short buffer_size;
	int ret;
	int len = strlen(cmdline) + 1;

	fd = open(image, O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open image file (rw mode)");
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

	ret = set_cmdline(argv[1], argv[2]);
	if (ret == 0)
		printf("Command line sucessfully modified\n");

	return ret;
}