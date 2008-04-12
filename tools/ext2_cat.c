/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <libext2.h>
#include <libstream.h>

#include "device.h"

int main(int argc, char **argv)
{
	char *path;
	device_io_t device;
	ext2_FILE* file;
	ext2_VOLUME *volume;
	char buffer[512];
	ssize_t size;
	int arg = 1;
	char *devname;

	if (argc <= arg)
		return 1;
	devname = argv[arg++];

	if (argc > arg)
		path = argv[arg++];
	else
		path = "/";

	device_sector_size = 512;
	device.data = (void*)device_open(devname, O_RDONLY);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;

	volume = ext2_mount(&device);
	if (volume == NULL)
		return 1;

	file = ext2_open(volume, path);
	if (file == NULL)
	{
		fprintf(stderr, "%s not found\n", path);
		return -1;
	}

	while((size = ext2_read(file, buffer, 512)) > 0)
		write(STDOUT_FILENO, buffer, size);
	ext2_close(file);

	ext2_umount(volume);

	device_close(device.data);

	return 0;
}
