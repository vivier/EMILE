/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <macos/memory.h>

void *malloc(size_t size)
{
	return NewPtr(size);
}
