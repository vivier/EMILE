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

#include <libiso9660.h>

#include "device.h"

static void list(iso9660_VOLUME *volume, char *path)
{
	char            name_buf[256];
	iso9660_DIR *dir;
	struct iso_directory_record *idr;

	dir = iso9660_opendir(volume, path);
	if (dir == NULL)
		return;

	while ((idr = iso9660_readdir(dir)) != NULL)
	{
		iso9660_name(volume->ucs_level, name_buf, idr);

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
	iso9660_VOLUME *volume;
	int arg = 1;

	if (argc > 1)
		device.data = device_open(argv[arg++]);
	else
		device.data = device_open("/dev/cdrom");
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;

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
