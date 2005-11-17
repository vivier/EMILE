/*
 *
 * (c) 2004, 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */
                                                                                
#include <sys/types.h>
#include <macos/scsi.h>

extern int scsi_command(int target, char* cdb, int count, TIB_t* tib);
extern int scsi_INQUIRY(int target, char* buffer, size_t count);
extern int scsi_READ(int target, unsigned long offset, unsigned short nb_blocks,
		     char *buffer, int buffer_size);

extern int scsi_read_sector(off_t offset, void* buffer, size_t size);
