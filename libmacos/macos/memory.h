/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_MEMORY_H__
#define __MACOS_MEMORY_H__

#include <macos/types.h>
#include <macos/errors.h>
#include <macos/traps.h>

#ifdef __mc68000__
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

typedef struct MemoryBlock {
	void		*address;
	unsigned long	count;
} MemoryBlock;

typedef struct LogicalToPhysicalTable {
	MemoryBlock logical;
	MemoryBlock physical[8];
} LogicalToPhysicalTable;

static inline OSErr LockMemory(void *address, unsigned long count)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
            "move.l %2, %%a1\n"
		MemoryDispatch(_LockMemory)
	    : "=d" (ret) : "a" (address), "a" (count)
	    : UNPRESERVED_REGS );

	return ret;
}

static inline OSErr LockMemoryContiguous(void *address, unsigned long count)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
            "move.l %2, %%a1\n"
		MemoryDispatch(_LockMemoryContiguous)
	    : "=d" (ret) : "a" (address), "a" (count)
	    : UNPRESERVED_REGS );

	return ret;
}

static inline OSErr GetPhysical(LogicalToPhysicalTable *addresses,
				unsigned long *physicalEntryCount)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
            "move.l %2, %%a1\n"
		MemoryDispatch(_GetPhysical)
	    : "=d" (ret) : "a" (addresses), "a" (physicalEntryCount)
	    : UNPRESERVED_REGS );

	return ret;
}

#endif /* __mc68000__ */
#endif /* __MACOS_MEMORY_H__ */
