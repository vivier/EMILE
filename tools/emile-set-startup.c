/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "libemile.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s <partition>\n", argv[0]);
	fprintf(stderr, "\n     Set startup partition\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int main(int argc, char** argv)
{
	int ret;
	int disk;
	int partition;
	char disk_name[16];

	if (argc != 2)
	{
		usage(argc, argv);
		return 1;
	}

	ret = emile_scsi_get_rdev(argv[1], &disk, &partition);
	if (ret == -1)
	{
		fprintf(stderr, "ERROR: cannot find disk of %s\n", argv[1]);
		return 1;
	}

	sprintf(disk_name, "/dev/sd%c", 'a' + disk);

	printf("Setting startup partition : %s%d\n", disk_name, partition);

	ret = emile_map_set_startup(disk_name, partition - 1);
	if (ret == -1) {
		fprintf(stderr, "ERROR: cannot set startup partition\n");
		return 1;
	}

	return 0;
}
