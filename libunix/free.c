#include <macos/memory.h>

void free(void *ptr)
{
	DisposePtr(ptr);
}
