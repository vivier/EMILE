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


#include <libiso9660.h>

#include "device.h"

static void list(stream_VOLUME *volume, char *path)
{
	char            name_buf[256];
	stream_DIR *dir;
	struct iso_directory_record *idr;

	dir = iso9660_opendir(volume, path);
	if (dir == NULL)
		return;

	while ((idr = iso9660_readdir(dir)) != NULL)
	{
		iso9660_name(volume, idr, name_buf);

		if (iso9660_is_directory(idr)) {
			printf("%s/\n", name_buf);
		} else {
			printf("%s\n", name_buf);
		}
	}
	iso9660_closedir(dir);
}

int main(int argc, char **argv)
{
	char *path;
	device_io_t device;
	stream_VOLUME *volume;
	int arg = 1;

	device_sector_size = 2048;
	if (argc > 1)
		device.data = (void*)device_open(argv[arg++], O_RDONLY);
	else
		device.data = (void*)device_open("/dev/cdrom", O_RDONLY);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;

	volume = iso9660_mount(&device);
	if (volume == NULL)
		return -1;

	if (argc > arg)
		path = argv[arg];
	else
		path = "/";

	list(volume, path);

	iso9660_umount(volume);

	device_close(device.data);

	return (0);
}
