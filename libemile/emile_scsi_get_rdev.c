static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

#define MAJOR_HD	3
#define MAJOR_SD	8
static char *scsi_base = "/dev/sd";
static char *ide_base = "/dev/hd";

int emile_scsi_get_rdev(char* dev_name, char** driver, int *disk, int *partition)
{
	int ret;
	struct stat st;
	int major;

	ret = stat(dev_name, &st);
	if (ret == -1)
		return -1;

	if (!S_ISBLK(st.st_mode))
		/*  not a block device */
		return -1;

	major = (st.st_rdev >> 8) & 0x0F;
	switch(major)
	{
	case MAJOR_SD:
		*driver = scsi_base;
		*disk = (st.st_rdev & 0xFF) >> 4;
		*partition = st.st_rdev &  0x0F;
		break;
	case MAJOR_HD:
		*driver = ide_base;
		*disk = (st.st_rdev & 0xFF) >> 6;
		*partition = st.st_rdev &  0x3F;
		break;
	default:
		return -1;
	}

	return 0;
}
