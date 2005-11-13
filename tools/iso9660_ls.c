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

#include "device.h"

static void list(char *path)
{
	char            name_buf[256];
	iso9660_DIR *dir;
	struct iso_directory_record *idr;

	dir = iso9660_opendir(path);
	if (dir == NULL)
		return;

	while ((idr = iso9660_readdir(dir)) != NULL)
	{
		iso9660_name(name_buf, idr);

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

	device_open();

	iso9660_init(device_read);

	if (iso9660_mount(NULL) != 0)
		return -1;

	if (argc > 1)
		path = argv[1];
	else
		path = "/";

	list(path);

	iso9660_umount();

	device_close();

	return (0);
}
