static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_scsi_get_rdev(char* dev_name, int* driver, int *disk, int *partition)
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

	major = (st.st_rdev >> 8) & 0xFF;
	*driver = major;
	switch(major)
	{
	case MAJOR_SD:
		*disk = (st.st_rdev & 0xFF) >> 4;
		if (partition) *partition = st.st_rdev &  0x0F;
		break;
	case MAJOR_IDE0:
	case MAJOR_IDE1:
		*disk = (st.st_rdev & 0xFF) >> 6;
		if (partition) *partition = st.st_rdev &  0x3F;
		break;
	default:
		return -1;
	}

	return 0;
}
