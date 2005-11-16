/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

extern int device_open(void);
extern void device_close(void);
extern void device_read(off_t offset, void* buffer, size_t size);
