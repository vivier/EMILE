#include <sys/types.h>
#include <macos/memory.h>

void *malloc(size_t size)
{
	return NewPtr(size);
}