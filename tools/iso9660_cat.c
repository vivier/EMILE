/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <libiso9660.h>
#include <libstream.h>

#include "device.h"

int main(int argc, char **argv)
{
	char *path;
	device_io_t device;
	iso9660_FILE* file;
	iso9660_VOLUME *volume;
	char buffer[512];
	size_t size;
	int arg = 1;
	char *devname;

	if (argc > 2)
		devname = argv[arg++];
	else
		devname = "/dev/cdrom";

	device.data = device_open(devname);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;

	volume = iso9660_mount(&device);
	if (volume == NULL)
		return 1;

	if (argc > arg)
		path = argv[arg++];
	else
		path = "/";

	file = iso9660_open(volume, path);
	if (file == NULL)
	{
		fprintf(stderr, "%s not found\n", path);
		return -1;
	}

	while((size = iso9660_read(file, buffer, 512)) > 0)
		write(STDOUT_FILENO, buffer, size);
	iso9660_close(file);

	iso9660_umount(volume);

	device_close(device.data);

	return 0;
}
