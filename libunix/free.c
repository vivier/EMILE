/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <macos/memory.h>

void free(void *ptr)
{
	DisposePtr(ptr);
}
