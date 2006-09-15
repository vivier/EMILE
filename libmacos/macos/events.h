/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_EVENTS_H__
#define __MACOS_EVENTS_H__

#include <macos/traps.h>

typedef u_int32_t	KeyMap[4];

static inline void GetKeys(KeyMap theKeys)
{
	asm("move.l %0, -(%%sp)\n"
		Trap(_GetKeys)
	    :: "g" (theKeys) : "%%d0", UNPRESERVED_REGS );
}
#endif /* __MACOS_EVENTS_H__ */
