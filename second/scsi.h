/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */
                                                                                
extern int scsi_INQUIRY(int target, char* buffer, size_t count);
extern int scsi_READ(int target, unsigned long offset, unsigned short nb_blocks,
		     char *buffer, int buffer_size);
