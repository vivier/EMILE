/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <scsi/scsi.h>

#include "blocks.h"

#define MAJOR_SD	8	/* SCSI disks */

struct scsi_id {
	int dev;
	int host_unique_id;
};

static int get_scsi_path(int fd, unsigned char *host, unsigned char *channel, 
				 unsigned char *pun, unsigned char *lun)
{
	int ret;
	struct scsi_id path;

	ret = ioctl(fd, SCSI_IOCTL_GET_IDLUN, &path);

	*host = path.dev >> 24;
	*channel = path.dev >> 16;
	*lun = path.dev >> 8;
	*pun = path.dev;

	return ret;
}

static int get_device_info(int device, int *id, unsigned long *first_block, 
				       int *block_size)
{
	int fd;
	int ret;
	char dev_name[16];
	int major;
	int minor;
	struct hd_geometry geom;
	unsigned char host;
	unsigned char channel;
	unsigned char pun;
	unsigned char lun;

	major = (device >> 8) & 0x0F;	/* major number = driver id */
	minor = device & 0xFF;		/* minor number = disk id */
	switch(major)
	{
	case MAJOR_SD: /* SCSI disks */
		sprintf(dev_name, "/dev/sd%c%d", 'a' + (minor >> 4),
						 minor & 0x0F);
		break;

	default:
		fprintf(stderr, "Unknown device major number %d\n", major);
		return -1;
	}

	fd = open(dev_name, O_RDONLY);
	if (fd == 1) {
		fprintf(stderr, "Cannot open device %s (%s)\n", dev_name,
				 strerror(errno));
		return -1;
	}
	ret = get_scsi_path(fd, &host, &channel, &pun, &lun);
	*id = pun;

	ret = ioctl(fd, HDIO_GETGEO, &geom);
	if (ret == -1)
	{
		fprintf(stderr, "%s: ioctl(HDIO_GETGEO) fails: %s",
				dev_name, strerror(errno));
		return -1;
	}
#if 0	/* BLKSSZGET is buggy on my m68k 2.2.27-pre2 kernel */
	ret = ioctl(fd, BLKSSZGET, block_size);
	if (ret == -1)
	{
		fprintf(stderr, "%s: ioctl(BLKSSZGET) fails: %s",
				dev_name, strerror(errno));
		return -1;
	}
#else
	*block_size = 512;
#endif
	*first_block = geom.start;
	close(fd);

	return 0;
}

#define ADD_BLOCK(a, b)					\
	container->blocks[current].offset = (a);	\
	container->blocks[current].count = (b);		\
	current++;					\
	if (current > container->max_blocks)		\
	{						\
		fprintf(stderr, "Container overflow\n");\
		return -1;				\
	}

int scsi_create_container(int fd, struct emile_container* container)
{
	int ret;
	struct stat st;
	int id;
	unsigned long first_block;
	int sector_size;
	int block_size;
	int sectors_per_block;
	int current;
	int logical;
	int physical;
	int last_physical;
	int num_blocks;
	int zone;
	int aggregate;
	int dev;

	ret = fstat(fd, &st);
	if (ret == -1) {
		perror("stat()");
		return -1;
	}

	dev = S_ISREG(st.st_mode) ? st.st_dev : st.st_rdev;

	ret = get_device_info(dev, &id, &first_block, &sector_size);
	if (ret != 0)
		return -1;

	container->unit_id = (u_int16_t)id;
	container->block_size = (u_int16_t)sector_size;

	/* get filesystem block size */

	ret = ioctl(fd, FIGETBSZ, &block_size);
	if (ret != 0) {
		perror("ioctl(FIGETBSZ)");
		return -1;
	}

	sectors_per_block = block_size / sector_size;

	/* get first physical block */

	last_physical = 0;
	ret = ioctl(fd, FIBMAP, &last_physical);
	if (ret != 0) {
		perror("ioctl(FIBMAP)");
		return -1;
	}

	zone = last_physical;
	aggregate = 1;

	/* seek all physical blocks */

	num_blocks = (st.st_size + st.st_blksize - 1) / st.st_blksize;
	current = 0;
	for (logical = 1; logical < num_blocks; logical++) {
		physical = logical;
		ret = ioctl(fd, FIBMAP, &physical);
		if (ret != 0)
			break;
		if (physical == last_physical + 1) {
			aggregate++;
		} else {
			ADD_BLOCK(first_block + zone * sectors_per_block,
				  aggregate * sectors_per_block);
			zone = physical;
			aggregate = 1;
		}
		last_physical = physical;
	}

	ADD_BLOCK(first_block + zone * sectors_per_block,
		  aggregate * sectors_per_block);

	/* end of list */

	ADD_BLOCK(0, 0);

	return 0;
}
