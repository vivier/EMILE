static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "libemile.h"

int emile_get_dev_name(char *buffer, int driver, int disk, int partition)
{
	switch(driver)
	{
		case MAJOR_IDE0:
			if (partition == 0)
				sprintf(buffer, "/dev/hd%c", 'a' + disk);
			else
				sprintf(buffer, "/dev/hd%c%d", 'a' + disk, 
						partition);
			break;
		case MAJOR_IDE1:
			if (partition == 0)
				sprintf(buffer, "/dev/hd%c", 'c' + disk);
			else
				sprintf(buffer, "/dev/hd%c%d", 'c' + disk, 
						partition);
			break;
		case MAJOR_LOOP:
			sprintf(buffer, "/dev/loop%d", disk);
			break;
		case MAJOR_SD:
			if (partition == 0)
				sprintf(buffer, "/dev/sd%c", 'a' + disk);
			else
				sprintf(buffer, "/dev/sd%c%d", 'a' + disk, 
						partition);
			break;
		default:
			return -1;
	}
	return 0;
}

int emile_scsi_get_dev(int fd, int* driver, int *disk, int *partition)
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

	major = (dev >> 8) & 0xFF;	/* major number = driver id */
	minor = dev & 0xFF;		/* minor number = disk id */

	*driver = major;
	switch(major)
	{
	case MAJOR_SD:	/* SCSI disks */
		*disk = minor >> 4;
		*partition = minor & 0x0F;
		break;
	case MAJOR_LOOP: /* loop device */
		*disk = minor & 0xFF;
		*partition = 0;
		break;
	case MAJOR_IDE0:
		*disk = minor >> 6;
		*partition = minor & 0x3F;
		break;
	case MAJOR_IDE1:
		*disk = minor >> 6;
		*partition = minor & 0x3F;
		break;
	default:
		fprintf(stderr, "Unknown device major number %d\n", major);
		return -1;
	}

	return 0;
}
