/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define SECTOR_SIZE     (2048)
#define ISO_BLOCKS(X)   (((X) / SECTOR_SIZE) + (((X)%SECTOR_SIZE)?1:0))

static const char *filename = "/dev/cdrom";
static FILE *infile = NULL;

int device_open(void)
{
	infile = fopen(filename, "rb");
	if (infile == NULL)
		return -1;
	return 0;
}

void device_close(void)
{
	if (infile)
		fclose(infile);
}

void device_read(off_t offset, void* buffer, size_t size)
{
	lseek(fileno(infile), offset << 11, SEEK_SET);
	read(fileno(infile), buffer, ISO_BLOCKS(size) << 11);
}
