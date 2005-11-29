static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "libemile.h"

/* SCSI disks */

#define MAJOR_HD	3
#define MAJOR_SD	8
static char *scsi_base = "/dev/sd";
static char *ata_base = "/dev/hd";

int emile_scsi_get_dev(int fd, char** driver, int *disk, int *partition)
{
	struct stat st;
	int ret;
	int dev;
	int major;
	int minor;

	ret = fstat(fd, &st);
	if (ret == -1)
		return -1;

	dev = S_ISREG(st.st_mode) ? st.st_dev : st.st_rdev;

	major = (dev >> 8) & 0x0F;	/* major number = driver id */
	minor = dev & 0xFF;		/* minor number = disk id */

	switch(major)
	{
	case MAJOR_SD:	/* SCSI disks */
		*driver = scsi_base;
		*disk = minor >> 4;
		*partition = minor & 0x0F;
		break;
	case MAJOR_HD:	/* ATA disks */
		*driver = ata_base;
		*disk = minor >> 6;
		*partition = minor & 0x3F;
		break;
	default:
		fprintf(stderr, "Unknown device major number %d\n", major);
		return -1;
	}

	return 0;
}
