/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_OSUTILS_H__
#define __MACOS_OSUTILS_H__

#include <macos/types.h>
#include <macos/traps.h>

struct QElem {
	struct QElem	*qLink;
	int16_t		qType;
	short		qData[1];
};
typedef struct QElem	QElem;
typedef QElem *		QElemPtr;

struct QHdr {
  volatile int16_t	qFlags;
  volatile QElemPtr	qHead;
  volatile QElemPtr	qTail;
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

static inline OSErr FlushInstructionCache(void)
{
	register OSErr ret asm("d0");

	asm(HWPriv(_FlushInstructionCache) ::: UNPRESERVED_REGS);

	return ret;
}

static inline OSErr FlushDataCache(void)
{
	register OSErr ret asm("d0");

	asm(HWPriv(_FlushDataCache) ::: UNPRESERVED_REGS);

	return ret;
}

static inline OSErr DisableExtCache(void)
{
	register OSErr ret asm("d0");

	asm(HWPriv(_DisableExtCache) ::: UNPRESERVED_REGS);

	return ret;
}

static inline OSErr FlushExtCache(void)
{
	register OSErr ret asm("d0");

	asm(HWPriv(_FlushExtCache) ::: UNPRESERVED_REGS);

	return ret;
}

static inline OSErr FlushCodeCacheRange(void *address, unsigned long count)
{
	register OSErr ret asm("d0");

	asm("move.l %0, %%a0\n"
	    "move.l %1, %%a1\n"
	    HWPriv(_FlushCodeCacheRange)
	    :: "g" (address), "g" (count) : UNPRESERVED_REGS);

	return ret;
}

#endif /* __mc68000__ */
#endif /* __MACOS_OSUTILS_H__ */
