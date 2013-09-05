/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_SLOTMANAGER_H__
#define __MACOS_SLOTMANAGER_H__
#include <macos/traps.h>

enum {
	sRsrcType	= 1,	/* Type of sResource */
	sRsrcName	= 2,	/* Name of sResource */
	sRsrcIcon	= 3,	/* Icon */
	sRsrcDrvrDir	= 4,	/* Driver Directory */
	sRsrcLoadDir	= 5,	/* Load directory */
	sRsrcBootRec	= 6,	/* sBoot record */
	sRsrcFlags	= 7,	/* sResourceFlags */
	sRsrcHWDevId	= 8,	/* Hardware Device ID */
};

struct SpBlock {
	long	spResult;
	char*	spsPointer;
	long	spSize;
	long	spOffsetData;
	char*	spIOFileName;
	char*	spsExecPBlk;
	long	spParamData;
	long	spMisc;
	long	spReserved;
	short	spIOReserved;
	short	spRefNum;
	short	spCategory;
	short	spCType;
	short	spDrvrSW;
	short	spDrvrHW;
	int8_t	spTBMask;
	int8_t	spSlot;
	int8_t	spID;
	int8_t	spExtDev;
	int8_t	spHwDev;
	int8_t	spByteLanes;
	int8_t	spFlags;
	int8_t	spKey;
} __attribute__((packed)) ;
typedef struct SpBlock                  SpBlock;
typedef SpBlock *                       SpBlockPtr;

enum { /* flags for spParamData */
	fall		= 0,
	foneslot	= 1,
	fnext		= 2,
};

#ifdef __mc68000__
static inline OSErr SGetCString(SpBlockPtr spBlkPtr)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		SlotManager(_SGetCString)
	    : "=g" (ret) : "g" (spBlkPtr) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr SGetTypeSRsrc(SpBlockPtr spBlkPtr)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		SlotManager(_SGetTypeSRsrc)
	    : "=g" (ret) : "g" (spBlkPtr) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr SRsrcInfo(SpBlockPtr spBlkPtr)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		SlotManager(_SRsrcInfo)
	    : "=g" (ret) : "g" (spBlkPtr) : UNPRESERVED_REGS);

	return ret;
}
#endif /* __mc68000__ */
#endif /* __MACOS_SLOTMANAGER_H__ */
