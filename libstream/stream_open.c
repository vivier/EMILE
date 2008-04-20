/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
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
#ifdef EXT2_SUPPORT
#include <libext2.h>
#endif
#ifdef MAP_SUPPORT
#include <libmap.h>
#endif

stream_init_t fs_init[] = {
#ifdef EXT2_SUPPORT
	[fs_EXT2] = ext2_init,
#endif
#ifdef ISO9660_SUPPORT
	[fs_ISO9660] = iso9660_init,
#endif
#ifdef CONTAINER_SUPPORT
	[fs_CONTAINER] = container_init,
#endif
#ifdef BLOCK_SUPPORT
	[fs_BLOCK] = block_init,
#endif
};

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
	if (strncmp("ext2:", path, 8) == 0)
	{
		*fs = fs_EXT2;
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

	stream->fs_id = fs,
	stream->device_id = device;
	stream->unit = unit;
	stream->partition = partition;

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
	if (partition != -1)
	{
#ifdef MAP_SUPPORT
		if (map_init(&stream->device, partition) == -1)
		{
		        printf("Cannot open map\n");
			stream->device.close(stream->volume);
			free(stream);
			return NULL;
		}
#else
		stream->device.close(stream->device.data);
		free(stream);
		return NULL;
#endif /* MAP_SUPPORT */
	}

	if (fs >= fs_LAST || fs_init[fs](&stream->device, &stream->fs) == -1)
	{
		stream->device.close(stream->device.data);
		free(stream);
		return NULL;
	}
	stream->volume = stream->fs.mount(&stream->device);
	if (stream->volume == NULL) {
		stream->device.close(stream->device.data);
		free(stream);
		return NULL;
	}
	stream->file = stream->fs.open(stream->volume, current);
	if (stream->file == NULL) {
		stream->fs.umount(stream->volume);
		stream->device.close(stream->device.data);
		free(stream);
		return NULL;
	}

	return stream;
}
