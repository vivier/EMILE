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


#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>
#include <libext2.h>

#include "device.h"

static void list(stream_VOLUME *volume, char *path)
{
	stream_DIR *dir;
	struct ext2_dir_entry_2 *entry;

	dir = ext2_opendir(volume, path);
	if (dir == NULL)
		return;
	while ((entry = ext2_readdir(dir)) != NULL)
	{
		if (entry->file_type == EXT2_FT_DIR)
			printf("%s/\n", entry->name);
		else
			printf("%s\n", entry->name);
	}
	ext2_closedir(dir);
}

int main(int argc, char **argv)
{
	char *path;
	device_io_t device;
	stream_VOLUME *volume;
	int arg = 1;

	device_sector_size = 512;
	if (argc <= 1) {
		fprintf(stderr, "You must specify a device to open\n");
		exit(1);
	}
	device.data = (void*)device_open(argv[arg++], O_RDONLY);
	device.read_sector = (stream_read_sector_t)device_read_sector;
	device.close = (stream_close_t)device_close;
	device.get_blocksize = (stream_get_blocksize_t)device_get_blocksize;

	volume = ext2_mount(&device);
	if (volume == NULL)
		return -1;

	if (argc > arg)
		path = argv[arg];
	else
		path = "/";

	list(volume, path);

	ext2_umount(volume);

	device_close(device.data);

	return (0);
}
