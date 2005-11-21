/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */
#ifndef __LIBFLOPY_H__
#define __LIBFLOPY_H__

#include <sys/types.h>
#include <unistd.h>

#define SECTOR_SIZE_BITS	9
#define SECTOR_SIZE		(1 << (SECTOR_SIZE_BITS))
#define SECTOR_PER_TRACK	18
#define SIDE_NB			2

typedef struct {
	int unit;
} floppy_device_t;

extern floppy_device_t *floppy_open(int unit);
extern int floppy_close(floppy_device_t* device);
extern int floppy_read_sector(floppy_device_t *device,off_t offset, void* buffer, size_t size);
#endif /* __LIBFLOPY_H__ */
