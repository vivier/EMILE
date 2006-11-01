/*
 *
 * (c) 2004, 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */
                                                                                
#include <sys/types.h>
#include <macos/scsi.h>

typedef struct {
	int target;
	unsigned int sector_size;
	unsigned long capacity;
} scsi_device_t;

extern int scsi_command(int target, char* cdb, int count, TIB_t* tib);
extern int scsi_INQUIRY(int target, char* buffer, size_t count);
extern int scsi_READ(int target, unsigned long offset, unsigned short nb_blocks,
		     char *buffer, int buffer_size);
extern int scsi_READ_CAPACITY(int target, char *buffer, size_t count);
extern int scsi_TEST_UNIT_READY(int target);
extern int scsi_REQUEST_SENSE(int target, char* buffer, size_t count);

extern scsi_device_t *scsi_open(int target);
extern int scsi_read_sector(scsi_device_t *device, off_t offset, void* buffer, size_t size);
extern int scsi_close(scsi_device_t *device);
extern int scsi_get_blocksize(scsi_device_t *device);
