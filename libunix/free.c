/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <macos/memory.h>

void free(void *ptr)
{
	DisposePtr(ptr);
}
