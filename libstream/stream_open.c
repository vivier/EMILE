/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libstream.h"
#include <libfloppy.h>
#include <libscsi.h>
#include <libblock.h>
#include <libcontainer.h>
#include <libiso9660.h>

typedef enum {
	device_FLOPPY,
	device_SCSI,
} device_t;

typedef enum {
	fs_BLOCK,
	fs_CONTAINER,
	fs_ISO9660,
} fs_t;

static char* get_fs(char *path, fs_t *fs)
{
	if (strncmp("block:", path, 6) == 0)
	{
		*fs = fs_BLOCK;
		return path + 6;
	} else if (strncmp("container:", path, 10) == 0)
	{
		*fs = fs_CONTAINER;
		return path + 10;
	} else if (strncmp("iso9660:", path, 8) == 0)
	{
		*fs = fs_ISO9660;
		return path + 8;
	}
	return NULL;
}

static char *get_device(char* path, 
		device_t *device, int *unit, int* partition)
{
	int nb;

	if (*path != '(')
		return NULL;
	path++;

	if (strncmp("fd", path, 2) == 0) {
		*device = device_FLOPPY;
		path += 2;
	} else if (strncmp("sd", path, 2) == 0) {
		*device = device_SCSI;
		path += 2;
	} else
		return NULL;

	nb = 0;
	while ( (*path >= '0') && (*path <= '9') ) {
		nb = (nb * 10) + (*path - '0');
		path++;
	}
	*unit = nb;

	*partition = -1;
	if ( (*path == 0) || (*path == ')') )
	{
		path++;
		return path;
	}

	if (*path != ',')
		return NULL;
	path++;

	nb = 0;
	while ( (*path >= '0') && (*path <= '9') ) {
		nb = (nb * 10) + (*path - '0');
		path++;
	}
	*partition = nb;
	if ( (*path == 0) || (*path == ')') )
	{
		path++;
		return path;
	}

	return NULL;
}

stream_t *stream_open(char *dev)
{
	stream_t *stream;
	fs_t fs;
	device_t device;
	int unit, partition;
	char *current;

	current = get_fs(dev, &fs);
	if (current == NULL)
	{
		printf("Cannot identify given filesystem\n");
		return NULL;
	}
	current = get_device(current, &device, &unit, &partition);
	if (current == NULL)
	{
		printf("Cannot identify given device\n");
		return NULL;
	}

	stream = (stream_t*)malloc(sizeof(stream_t));

	switch(device)
	{
#if 0
		case device_FLOPPY:
			if (partition != -1)
			{
				free(stream);
				return NULL;
			}
			stream->device.data = floppy_open(unit);
			if (stream->device.data == NULL)
			{
				free(stream);
				return NULL;
			}
			stream->device.read_sector = (stream_read_sector_t)floppy_read_sector;
			stream->device.close = (stream_close_t)floppy_close;
			break;
#endif

		case device_SCSI:
			stream->device.data = scsi_open(unit);
			if (stream->device.data == NULL)
			{
				free(stream);
				return NULL;
			}
			stream->device.read_sector = (stream_read_sector_t)scsi_read_sector;
			stream->device.close = (stream_close_t)scsi_close; 
			break;
		default:
			free(stream);
			stream = NULL;
			break;
	}

	switch(fs)
	{
#if 0
		case fs_BLOCK:
			if (stream->fs.data == NULL)
				goto outfs;
			stream->fs.read = (stream_read_t)block_read;
			stream->fs.seek = (stream_lseek_t)block_seek;
			stream->fs.close = (stream_close_t)block_close;
			stream->fs.umount = (stream_umount_t)block_umount;
			stream->fs.fstat = (stream_fstat_t)block_fstat;
			break;
		case fs_CONTAINER:
			stream->fs.data = container_open(&stream->device, current);
			if (stream->fs.data == NULL)
				goto outfs;
			stream->fs.read = (stream_read_t)container_read;
			stream->fs.seek = (stream_lseek_t)container_seek;
			stream->fs.close = (stream_close_t)container_close;
			stream->fs.umount = (stream_umount_t)container_umount;
			stream->fs.fstat = (stream_fstat_t)container_fstat;
			break;
#endif
		case fs_ISO9660:
			stream->fs.volume = iso9660_mount(&stream->device);
			if (stream->fs.volume == NULL)
			{
				printf("Cannot mount volume ISO9660\n");
				goto outfs;
			}
			stream->fs.file = iso9660_open(stream->fs.volume, current);
			if (stream->fs.file == NULL)
			{
				iso9660_umount(stream->fs.volume);
				goto outfs;
			}
			stream->fs.read = (stream_read_t)iso9660_read;
			stream->fs.lseek = (stream_lseek_t)iso9660_lseek;
			stream->fs.close = (stream_close_t)iso9660_close;
			stream->fs.umount = (stream_umount_t)iso9660_umount;
			stream->fs.fstat = (stream_fstat_t)iso9660_fstat;
			break;
		default:
outfs:
			stream->device.close(stream->device.data);
			free(stream);
			return NULL;
	}

	return stream;
}
