/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <macos/memory.h>

void free(void *ptr)
{
	DisposePtr(ptr);
}
