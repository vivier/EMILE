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

int main(int argc, char **argv)
{
	char *path;
	iso9660_FILE* file;
	char buffer[512];
	size_t size;

	device_open();

	iso9660_init(device_read);

	if (iso9660_mount(NULL) != 0)
		return 1;

	if (argc > 1)
		path = argv[1];
	else
		path = "/";

	file = iso9660_open(path);
	if (file == NULL)
	{
		fprintf(stderr, "%s not found\n", path);
		return -1;
	}

	while((size = iso9660_read(file, buffer, 512)) > 0)
		write(STDOUT_FILENO, buffer, size);
	iso9660_close(file);

	iso9660_umount();

	device_close();

	return 0;
}
