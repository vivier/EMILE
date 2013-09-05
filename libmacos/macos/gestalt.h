/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_GESTALT_H__
#define __MACOS_GESTALT_H__

#include <macos/traps.h>

enum {
	gestaltProcessorType	= 'proc',
	gestalt68000		= 1,
	gestalt68010		= 2,
	gestalt68020		= 3,
	gestalt68030		= 4,
	gestalt68040		= 5
};

enum {
	gestaltMMUType		= 'mmu ', /* mmu type */
	gestaltNoMMU		= 0,	/* no MMU */
	gestaltAMU		= 1,	/* address management unit */
	gestalt68851		= 2,	/* 68851 PMMU */
	gestalt68030MMU		= 3,	/* 68030 built-in MMU */
	gestalt68040MMU		= 4,	/* 68040 built-in MMU */
	gestaltEMMU1		= 5	/* Emulated MMU type 1  */
};

enum {
	gestaltFPUType	= 'fpu ', /* fpu type */
	gestaltNoFPU	= 0,	/* no FPU */
	gestalt68881	= 1,	/* 68881 FPU */
	gestalt68882	= 2,	/* 68882 FPU */
	gestalt68040FPU	= 3	/* 68040 built-in FPU */
};

enum {
	gestaltSysArchitecture	= 'sysa',  /* Native System Architecture */
	gestalt68k		= 1,	   /* Motorola MC68k architecture */
	gestaltPowerPC		= 2	   /* IBM PowerPC architecture */
};

enum {
  gestaltMachineType            = 'mach',	/* machine type */
  gestaltClassic                = 1,
  gestaltMacXL                  = 2,
  gestaltMac512KE               = 3,
  gestaltMacPlus                = 4,
  gestaltMacSE                  = 5,
  gestaltMacII                  = 6,
  gestaltMacIIx                 = 7,
  gestaltMacIIcx                = 8,
  gestaltMacSE030               = 9,
  gestaltPortable               = 10,
  gestaltMacIIci                = 11,
  gestaltPowerMac8100_120       = 12,
  gestaltMacIIfx                = 13,
  gestaltMacClassic             = 17,
  gestaltMacIIsi                = 18,
  gestaltMacLC                  = 19,
  gestaltMacQuadra900           = 20,
  gestaltPowerBook170           = 21,
  gestaltMacQuadra700           = 22,
  gestaltClassicII              = 23,
  gestaltPowerBook100           = 24,
  gestaltPowerBook140           = 25,
  gestaltMacQuadra950           = 26,
  gestaltMacLCIII               = 27,
  gestaltPerforma450            = gestaltMacLCIII,
  gestaltPowerBookDuo210        = 29,
  gestaltMacCentris650          = 30,
  gestaltPowerBookDuo230        = 32,
  gestaltPowerBook180           = 33,
  gestaltPowerBook160           = 34,
  gestaltMacQuadra800           = 35,
  gestaltMacQuadra650           = 36,
  gestaltMacLCII                = 37,
  gestaltPowerBookDuo250        = 38,
  gestaltAWS9150_80             = 39,
  gestaltPowerMac8100_110       = 40,
  gestaltAWS8150_110            = gestaltPowerMac8100_110,
  gestaltPowerMac5200           = 41,
  gestaltPowerMac5260           = gestaltPowerMac5200,
  gestaltPerforma5300           = gestaltPowerMac5200,
  gestaltPowerMac6200           = 42,
  gestaltPerforma6300           = gestaltPowerMac6200,
  gestaltMacIIvi                = 44,
  gestaltMacIIvm                = 45,
  gestaltPerforma600            = gestaltMacIIvm,
  gestaltPowerMac7100_80        = 47,
  gestaltMacIIvx                = 48,
  gestaltMacColorClassic        = 49,
  gestaltPerforma250            = gestaltMacColorClassic,
  gestaltPowerBook165c          = 50,
  gestaltMacCentris610          = 52,
  gestaltMacQuadra610           = 53,
  gestaltPowerBook145           = 54,
  gestaltPowerMac8100_100       = 55,
  gestaltMacLC520               = 56,
  gestaltAWS9150_120            = 57,
  gestaltPowerMac6400           = 58,
  gestaltPerforma6400           = gestaltPowerMac6400,
  gestaltPerforma6360           = gestaltPerforma6400,
  gestaltMacCentris660AV        = 60,
  gestaltMacQuadra660AV         = gestaltMacCentris660AV,
  gestaltPerforma46x            = 62,
  gestaltPowerMac8100_80        = 65,
  gestaltAWS8150_80             = gestaltPowerMac8100_80,
  gestaltPowerMac9500           = 67,
  gestaltPowerMac9600           = gestaltPowerMac9500,
  gestaltPowerMac7500           = 68,
  gestaltPowerMac7600           = gestaltPowerMac7500,
  gestaltPowerMac8500           = 69,
  gestaltPowerMac8600           = gestaltPowerMac8500,
  gestaltAWS8550                = gestaltPowerMac7500,
  gestaltPowerBook180c          = 71,
  gestaltPowerBook520           = 72,
  gestaltPowerBook520c          = gestaltPowerBook520,
  gestaltPowerBook540           = gestaltPowerBook520,
  gestaltPowerBook540c          = gestaltPowerBook520,
  gestaltPowerMac5400           = 74,
  gestaltPowerMac6100_60        = 75,
  gestaltAWS6150_60             = gestaltPowerMac6100_60,
  gestaltPowerBookDuo270c       = 77,
  gestaltMacQuadra840AV         = 78,
  gestaltPerforma550            = 80,
  gestaltPowerBook165           = 84,
  gestaltPowerBook190           = 85,
  gestaltMacTV                  = 88,
  gestaltMacLC475               = 89,
  gestaltPerforma47x            = gestaltMacLC475,
  gestaltMacLC575               = 92,
  gestaltMacQuadra605           = 94,
  gestaltMacQuadra630           = 98,
  gestaltMacLC580               = 99,
  gestaltPerforma580            = gestaltMacLC580,
  gestaltPowerMac6100_66        = 100,
  gestaltAWS6150_66             = gestaltPowerMac6100_66,
  gestaltPowerMac6100_80	= 101,
  gestaltPowerBookDuo280        = 102,
  gestaltPowerBookDuo280c       = 103,
  gestaltPowerMacLC475          = 104,
  gestaltPowerMacPerforma47x    = gestaltPowerMacLC475,
  gestaltPowerMacLC575          = 105,
  gestaltPowerMacPerforma57x    = gestaltPowerMacLC575,
  gestaltPowerMacQuadra630      = 106,
  gestaltPowerMacLC630          = gestaltPowerMacQuadra630,
  gestaltPowerMacPerforma63x    = gestaltPowerMacQuadra630,
  gestaltPowerMac7200           = 108,
  gestaltPowerMac7300           = 109,
  gestaltPowerMac7100_66        = 112,
  gestaltPowerMac7100_80_chipped= 113,
  gestaltPowerBook150           = 115,
  gestaltPowerMacQuadra700      = 116,
  gestaltPowerMacQuadra900      = 117,
  gestaltPowerMacQuadra950      = 118,
  gestaltPowerMacCentris610     = 119,
  gestaltPowerMacCentris650     = 120,
  gestaltPowerMacQuadra610      = 121,
  gestaltPowerMacQuadra650      = 122,
  gestaltPowerMacQuadra800      = 123,
  gestaltPowerBookDuo2300       = 124,
  gestaltPowerBook500PPCUpgrade = 126,
  gestaltPowerBook5300          = 128,
  gestaltPowerBook1400          = 310,
  gestaltPowerBook3400          = 306,
  gestaltPowerBook2400          = 307,
  gestaltPowerBookG3Series      = 312,
  gestaltPowerBookG3            = 313,
  gestaltPowerBookG3Series2     = 314,
  gestaltPowerMacNewWorld       = 406,
  gestaltPowerMacG3             = 510,
  gestaltPowerMac5500           = 512,
  gestalt20thAnniversary        = gestaltPowerMac5500,
  gestaltPowerMac6500           = 513,
  gestaltPowerMac4400_160       = 514,
  gestaltPowerMac4400           = 515,
  gestaltMacOSCompatibility     = 1206
};

enum {
	gestaltHardwareAttr= 'hdwr',	/* hardware attributes */
	gestaltHasVIA1		= 0,	/* VIA1 exists */
	gestaltHasVIA2		= 1,	/* VIA2 exists */
	gestaltHasRBV          =  2,
	gestaltHasASC		= 3,	/* Apple Sound Chip exists */
	gestaltHasSCC		= 4,	/* SCC exists */
	gestaltHasOSS          =  5,
	gestaltHasSCSIDMA      =  6,
	gestaltHasSCSI		= 7,	/* SCSI exists */
	gestaltHasSWIMIOP      =  8,
	gestaltHasSCCIOP       =  9,
	gestaltHasFitch        = 10,
	gestaltHasIWM          = 11,
	gestaltHasPWM          = 12,
	gestaltHasRAMSndBuff   = 13,
	gestaltHasVideoDAConv  = 14,
	gestaltHasPGC          = 15,
	gestaltHasSoftPowerOff	= 19,	/* Capable of software power off */
	gestaltHasSonic        = 20,
	gestaltHasSCSI961	= 21,	/* 53C96 SCSI ctrl on internal bus */
	gestaltHasSCSI962	= 22,	/* 53C96 SCSI ctrl on external bus */
	gestaltHasDAFBVideo    = 23,
	gestaltHasUniversalROM	= 24,	/* Do we have a Universal ROM? */
	gestaltHasEnhancedLtalk	= 30	/* Do we have Enhanced LocalTalk? */
};

#ifdef __mc68000__
static inline OSErr Gestalt(OSType selector, long * response)
{
	register OSErr ret asm("%%d0");

	asm("move.l %2, %%d0\n"
		Trap(_Gestalt)
	    "	move.l %%a0, %1\n"
	    : "=d" (ret), "=m" (*response) : "g" (selector) : UNPRESERVED_REGS);

	return ret;
}
#endif /* __mc68000__ */
#endif /* __MACOS_GESTALT_H__ */
