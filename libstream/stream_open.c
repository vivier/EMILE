/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libstream.h"
#ifdef FLOPPY_SUPPORT
#include <libfloppy.h>
#endif
#ifdef SCSI_SUPPORT
#include <libscsi.h>
#endif
#ifdef BLOCK_SUPPORT
#include <libblock.h>
#endif
#ifdef CONTAINER_SUPPORT
#include <libcontainer.h>
#endif
#ifdef ISO9660_SUPPORT
#include <libiso9660.h>
#endif

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
	}
	if (strncmp("container:", path, 10) == 0)
	{
		*fs = fs_CONTAINER;
		return path + 10;
	}
	if (strncmp("iso9660:", path, 8) == 0)
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
#ifdef FLOPPY_SUPPORT
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
			stream->device.read_sector = 
				(stream_read_sector_t)floppy_read_sector;
			stream->device.close = (stream_close_t)floppy_close;
			stream->device.get_blocksize = 
				(stream_get_blocksize_t)floppy_get_blocksize; 
			break;
#endif /* FLOPPY_SUPPORT */

#ifdef SCSI_SUPPORT
		case device_SCSI:
			stream->device.data = scsi_open(unit);
			if (stream->device.data == NULL)
			{
				free(stream);
				return NULL;
			}
			stream->device.read_sector = 
				(stream_read_sector_t)scsi_read_sector;
			stream->device.close = (stream_close_t)scsi_close; 
			stream->device.get_blocksize = 
				(stream_get_blocksize_t)scsi_get_blocksize; 
			break;
#endif /* SCSI_SUPPORT */

		default:
			free(stream);
			return NULL;
			break;
	}

	switch(fs)
	{
#ifdef BLOCK_SUPPORT
		case fs_BLOCK:
			stream->fs.volume = NULL;
			stream->fs.file = block_open(&stream->device, current);
			if (stream->fs.file == NULL)
				goto outfs;
			stream->fs.read = (stream_read_t)block_read;
			stream->fs.lseek = (stream_lseek_t)block_lseek;
			stream->fs.close = (stream_close_t)block_close;
			stream->fs.umount = NULL;
			stream->fs.fstat = (stream_fstat_t)block_fstat;
			break;
#endif /* BLOCK_SUPPORT */
#ifdef CONTAINER_SUPPORT
		case fs_CONTAINER:
			stream->fs.volume = NULL;
			stream->fs.file = container_open(&stream->device, current);
			if (stream->fs.data == NULL)
				goto outfs;
			stream->fs.read = (stream_read_t)container_read;
			stream->fs.seek = (stream_lseek_t)container_seek;
			stream->fs.close = (stream_close_t)container_close;
			stream->fs.umount = (stream_umount_t)container_umount;
			stream->fs.fstat = (stream_fstat_t)container_fstat;
			break;
#endif /* CONTAINER_SUPPORT */

#ifdef ISO9660_SUPPORT
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
#endif /* ISO9660_SUPPORT */

		default:
outfs:
			stream->device.close(stream->device.data);
			free(stream);
			return NULL;
	}

	return stream;
}
