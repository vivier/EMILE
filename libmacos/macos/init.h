/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_INIT_H__
#define __MACOS_INIT_H__

#include <macos/traps.h>

#ifdef __mc68000__
statis inline void InitEvents(short CntEvts)
{
	asm("move.l %0, %%d0\n"
		Trap(_InitEvents)
	    :: "g" (CntEvts) : "%%d0", UNPRESERVED_REGS);
}

static inline void InitFS(short CntFCBs)
{
	asm("move.l %0, %%d0\n"
		Trap(_InitFs)
	    :: "g" (CntFCBs) : "%%d0", UNPRESERVED_REGS);
}
#endif /* __mc68000__ */
#endif /* __MACOS_INIT_H__ */
