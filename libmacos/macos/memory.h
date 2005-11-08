/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __MACOS_MEMORY_H__
#define __MACOS_MEMORY_H__

#include <macos/traps.h>

static inline void DisposePtr(void* ptr)
{
	asm("move.l %0, %%a0\n"
		Trap(_DisposePtr)
	    :: "g" (ptr) : "%%d0", UNPRESERVED_REGS);
}

static inline void* NewPtr(unsigned long byteCount)
{
	register void* ptr;

	asm("move.l %1, %%d0\n"
		Trap(_NewPtr)
	    "	move.l %%a0, %0\n"
	    : "=g" (ptr) : "g" (byteCount) : "%%d0", UNPRESERVED_REGS);

	return ptr;
}
#endif /* __MACOS_MEMORY_H__ */
