/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
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
#include <libstream.h>

#include "device.h"

int main(int argc, char **argv)
{
	char *path;
	device_io_t device;
	stream_FILE* file;
	stream_VOLUME *volume;
	char buffer[512];
	size_t size;
	int get_info = 0;
	int arg = 1;
	char *devname;

	if ((argc > arg) && (strcmp(argv[arg], "-i") == 0)) {
		arg++;
		get_info = 1;
	}

	if (argc > arg)
		devname = argv[arg++];
	else
		devname = "/dev/cdrom";

	if (argc > arg)
		path = argv[arg++];
	else
		path = "/";

	device_sector_size = 2048;
	device.data = (void*)device_open(devname, O_RDONLY);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;

	volume = iso9660_mount(&device);
	if (volume == NULL)
		return 1;

	file = iso9660_open(volume, path);
	if (file == NULL)
	{
		fprintf(stderr, "%s not found\n", path);
		return -1;
	}

	if (get_info) {
		struct stream_stat st;
		iso9660_fstat(file, &st);
		printf("%jd %jd\n", st.st_base * 4, st.st_size);
	} else {
		while((size = iso9660_read(file, buffer, 512)) > 0)
			write(STDOUT_FILENO, buffer, size);
	}
	iso9660_close(file);

	iso9660_umount(volume);

	device_close(device.data);

	return 0;
}
