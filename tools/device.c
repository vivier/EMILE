/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#define _LARGEFILE64_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

#include "device.h"

int device_sector_size = 2048;

#define BLOCKS(X)   (((X)/device_sector_size)+(((X)%device_sector_size)?1:0))

int device_get_blocksize(void *data)
{
	return device_sector_size;
}

int device_read_sector(void *data,off_t offset, void* buffer, size_t size)
{
	int fd = (long)data;
	int ret;

	lseek64(fd, (unsigned long long)offset * device_sector_size, SEEK_SET);
	ret = read(fd, buffer, BLOCKS(size) * device_sector_size);
	if (ret != BLOCKS(size) * device_sector_size)
		return -1;
	return 0;
}

int device_write_sector(void *data,off_t offset, void* buffer, size_t size)
{
	int fd = (long)data;
	int ret;

	lseek64(fd, (unsigned long long)offset * device_sector_size, SEEK_SET);
	ret = write(fd, buffer, BLOCKS(size) * device_sector_size);
	if (ret != BLOCKS(size) * device_sector_size)
		return -1;
	return 0;
}

void device_close(void *data)
{
	close((long)data);
}

long device_open(char *device, int flags)
{
	int fd;

	if (device == NULL)
		return -1;

	fd = open(device, flags);

	return fd;
}
