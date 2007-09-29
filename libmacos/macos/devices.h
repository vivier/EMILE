/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_DEVICES_H__
#define __MACOS_DEVICES_H__

#include <macos/traps.h>
#include <macos/types.h>
#include <macos/files.h>
#include <macos/errors.h>
#include <macos/osutils.h>

typedef struct
{
	short		drvrFlags;
	short		drvrDelay;
	short		drvrEMask;
	short		drvrMenu;
	short		drvrOpen;
	short		drvrPrime;
	short		drvrCtl;
	short		drvrStatus;
	short		drvrClose;
	unsigned char	drvrName[];
} DriverHeader;
enum {
dVMImmuneMask                 = 0x0001, /* driver does not need VM protection */
dOpenedMask                   = 0x0020, /* driver is open */
dRAMBasedMask                 = 0x0040, /* dCtlDriver is a handle (1) or pointer (0) */
drvrActiveMask                = 0x0080 /* driver is currently processing a request */
};

struct DCtlEntry {
	void*		dCtlDriver;
	volatile short	dCtlFlags;
	QHdr		dCtlQHdr;
	volatile long	dCtlPosition;
	void**		dCtlStorage;
	short		dCtlRefNum;
	long		dCtlCurTicks;
	void*		dCtlWindow;
	short		dCtlDelay;
	short		dCtlEMask;
	shorti		dCtlMenu;
};
typedef struct DCtlEntry                DCtlEntry;
typedef DCtlEntry *                     DCtlPtr;
typedef DCtlPtr *                       DCtlHandle;

#ifdef __mc68000__
static inline OSErr PBCloseSync(ParmBlkPtr paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_PBCloseSync)
	    : "=d" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr PBControlSync(ParmBlkPtr paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_PBControlSync)
	    : "=d" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr PBOpenSync(ParmBlkPtr paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_PBOpenSync)
	    : "=d" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr PBReadSync(ParamBlockRec_t* paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_PBReadSync)
	    : "=d" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr PBStatusSync(ParamBlockRec_t* paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_PBStatusSync)
	    : "=d" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr PBWriteSync(ParamBlockRec_t* paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_PBWriteSync)
	    : "=d" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr DrvrRemove(int refNum)
{
	register OSErr ret asm("%%d0");
	asm("move.l %1, %%d0\n"
		Trap(_DrvrRemove)
	    : "=d" (ret) : "g" (refNum) : UNPRESERVED_REGS);

	return ret;
}
#endif /* __mc68000__ */

extern OSErr OpenDriver(ConstStr255Param name, short *drvrRefNum);
extern OSErr CloseDriver(short refNum);
extern OSErr SerGetBuf(short refNum, long *count);
extern DCtlHandle GetDCtlEntry(short refNum);
#endif /* __MACOS_DEVICES_H__ */
