/*
 *
 * (c) 2004, 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_LOWMEM_H__
#define __MACOS_LOWMEM_H__

#include <macos/osutils.h>

#define VIA1Base	(*(unsigned long*)0x01D4)
#define SCCRd		(*(unsigned long*)0x01d8)
#define SCCWr		(*(unsigned long*)0x01dc)
#define ScrnBase	(*(unsigned long*)0x0824)
#define ROMBase		(*(short**)0x02ae)
#define HWCfgFlags	(*(unsigned long*)0x0B22)
#define SCSIBase	(*(unsigned long*)0x0C00)
#define ASCBase		(*(unsigned long*)0x0CC0)
#define VIA2Base	(*(unsigned long*)0x0CEC)
#define PMgrBase	(*(unsigned long*)0x0D18)
#define UnivROMBits	(*(unsigned long*)0x0DD4)
#define TimeVIADB	(*(unsigned short*)0x0cea)
#define TimeDBRA	(*(unsigned short*)0x0D00)
#define MemTop		(*(unsigned long*)0x0108)
#define Time		(*(unsigned long*)0x020C)
#define KeyLast		(*(unsigned short*)0x0184)
#define KeyTime		(*(unsigned long*)0x0186)
#define KeyRepTime	(*(unsigned long*)0x018A)
#define Ticks		(*(unsigned long*)0x016A)

#define LMGetUnitTableEntryCount()	(*(short*)0x1d2)
#define LMGetUTableBase()	(*(unsigned long*)0x11c)
#define LMGetDrvQHdr()		( (QHdrPtr) 0x0308)
#endif /* __MACOS_LOWMEM_H__ */
