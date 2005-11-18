/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>

#define SECTOR_SIZE_BITS	9
#define SECTOR_SIZE		(1 << (SECTOR_SIZE_BITS))
#define SECTOR_PER_TRACK	18
#define SIDE_NB			2
#define CYLINDER_SIZE		(SIDE_NB*SECTOR_PER_TRACK*SECTOR_SIZE)

typedef struct {
	int offset;
	int current_cylinder;
	unsigned char cylinder[CYLINDER_SIZE];
} floppy_FILE;

extern int floppy_read_sector(off_t offset, void* buffer, size_t size);
extern size_t floppy_read(floppy_FILE *file, void *ptr, size_t size);
extern int floppy_lseek(floppy_FILE *file, off_t offset, int whence);
