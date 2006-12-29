/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_OSUTILS_H__
#define __MACOS_OSUTILS_H__

#include <macos/types.h>
#include <macos/traps.h>

struct QHdr {
  int16_t      qFlags;
  void*   qHead;
  void*   qTail;
};
typedef struct QHdr                     QHdr;
typedef QHdr *                          QHdrPtr;

struct MachineLocation
{
	int32_t latitude;
	int32_t longitude;
	union
	{
		int8_t dlsDelta;
		int32_t gmtDelta;
	} u;
};
typedef struct MachineLocation MachineLocation;

#ifdef __mc68000__
static inline void ReadLocation(MachineLocation * loc)
{
	asm("move.l %0, %%a0\n"
		XPRam(_ReadLocation)
	    :: "g" (loc) : "%%d0", UNPRESERVED_REGS);
}
#endif /* __mc68000__ */
#endif /* __MACOS_OSUTILS_H__ */
