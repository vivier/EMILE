/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 *  a lot of parts from penguin booter
 *  based on bootstrap.c for Atari Linux booter, Copyright 1993 by Arjan Knor
 *
 */

#include <string.h>

#include "memory.h"
#include "glue.h"
#include "console.h"
#include "MMU.h"
#include "lowmem.h"
#include "misc.h"
#include "bootinfo.h"

extern unsigned char _ramdisk_start;
extern unsigned char _ramdisk_end;

static char* command_line = KERNEL_ARGS;

static struct bootinfo boot_info = { 0 };

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


#define noErr	0

#if defined(EXTENDED_HW_MAP)

static int
MacHasHardware(unsigned long gestaltBit)
{
	long	r;

	Gestalt(gestaltHardwareAttr, &r);

	return ( (r & (1 << gestaltBit)) != 0);
}
#endif /* EXTENDED_HW_MAP */

void bootinfo_init()
{
	long proc, fpu, mmu, mach, ram;
	int i;

	/* I'm a macintosh, I know, I'm sure */

	boot_info.machtype = MACH_MAC;

	/* get processor type */

	Gestalt(gestaltProcessorType, &proc);

	/* check FPU */

	if (Gestalt('FPUE', &fpu) == noErr)
		fpu = 0;
	else
		Gestalt(gestaltFPUType, &fpu);

	/* check MMU */

	Gestalt(gestaltMMUType, &mmu);

	/* check machine type */

	Gestalt(gestaltMachineType, &mach);
	boot_info.bi_mac.id = mach;

	/* check ram size */

	Gestalt('ram ', &ram);
	boot_info.bi_mac.memsize = ram / (1024L * 1024L);

	/* set processor type */

	switch (proc)
	{
		case gestalt68000:
			error("68000 has no MMU.");
			break;

		case gestalt68010:
			error("68010 has no MMU.");
			break;

		case gestalt68020:
			if (mmu != gestalt68851)
				error("68020 has no MMU.");
			else
				boot_info.cputype = CPU_68020;
			break;

		case gestalt68030:
			boot_info.cputype = CPU_68030;
			break;

		case gestalt68040:
			boot_info.cputype = CPU_68040;
			break;

		default:
			error("Unknown processor.");
			break;
	}
	boot_info.bi_mac.cpuid = proc - gestalt68020;

	/* Set the FPU info */

	switch (fpu)
	{
		case gestalt68881:
			boot_info.cputype |= FPU_68881;
			break;

		case gestalt68882:
			boot_info.cputype |= FPU_68882; break;
			break;

		case gestalt68040FPU:
			boot_info.cputype |= FPU_68040;
			break;

		case gestaltNoFPU:
		default:
			break;
	}

	/* memory structure */

	for (i = 0; i < memory_map.bank_number; i++)
	{
		boot_info.memory[i].addr = memory_map.bank[i].address;
		boot_info.memory[i].size = memory_map.bank[i].size;
	}
	boot_info.num_memory = i;

	/* ramdisk info */

	boot_info.ramdisk_size = (unsigned long)&_ramdisk_end - 
				 (unsigned long)&_ramdisk_start;
	boot_info.ramdisk_addr = (unsigned long)&_ramdisk_start;

	/* command line */

	strncpy(boot_info.command_line, command_line, CL_SIZE);

	/* macintosh */

	/* video information */

	boot_info.bi_mac.videological = console_get_videobase();
	logical2physical(console_get_videobase(), &boot_info.bi_mac.videoaddr);
	boot_info.bi_mac.videorow = console_get_row_bytes();
	boot_info.bi_mac.videodepth = console_get_depth();
	boot_info.bi_mac.dimensions = (console_get_height() << 16) 
							| console_get_width();

	/* boot time and time zone */

	boot_info.bi_mac.args = 0;
	boot_info.bi_mac.boottime = 0;
	boot_info.bi_mac.gmtbias = 0;

	/* booter version */

	boot_info.bi_mac.bootver = 108;

	logical2physical(SCCRd, &boot_info.bi_mac.scc);
	boot_info.bi_mac.timedbra = TimeDBRA;
	boot_info.bi_mac.adbdelay = TimeVIADB;
	boot_info.bi_mac.serialmf = 0;
	boot_info.bi_mac.serialhsk = 9600;
	boot_info.bi_mac.serialgpi = 9600;

	/* ROM base */

	boot_info.bi_mac.rombase = ROMBase;

#if defined(EXTENDED_HW_MAP)
	/* hardware information */

	boot_info.bi_mac.HwMap = 0;

	/* VIA1 */

	if (MacHasHardware(gestaltHasVIA1))
	{
		boot_info.bi_mac.HwMap |= HW_MAP_VIA1;
		logical2physical(VIA1Base, &boot_info.bi_mac.VIA1Base);
	}

	/* VIA2/RBV/OSS */

	if (MacHasHardware(gestaltHasVIA2))
		boot_info.bi_mac.HwMap |= HW_MAP_VIA2_VIA;
	else if (MacHasHardware(gestaltHasRBV))
		boot_info.bi_mac.HwMap |= HW_MAP_VIA2_RBV;
	else if (MacHasHardware(gestaltHasOSS))
		boot_info.bi_mac.HwMap |= HW_MAP_VIA2_OSS;
	
	if (boot_info.bi_mac.HwMap & 
		(HW_MAP_VIA2_VIA | HW_MAP_VIA2_RBV | HW_MAP_VIA2_OSS))
	{
		logical2physical(VIA2Base, &boot_info.bi_mac.VIA2Base);
	}

	/* ADB */

	if (MacHasHardware(gestaltHasSWIMIOP))
	{
		boot_info.bi_mac.HwMap |= HW_MAP_ADB_IOP;
	}
	else
	{
		unsigned long bits = UnivROMBits;
		if (bits != -1)
		{
			bits &= 0x07000000;
			bits >>= 24;

			if (bits != 0)
			{
				if ( (bits == 1) || (bits == 2) )
				{
					boot_info.bi_mac.HwMap |= HW_MAP_ADB_IISI;
				}
				else
					boot_info.bi_mac.HwMap |= HW_MAP_ADB_CUDA;
			}
			else
			{
				if (PMgrBase != -1)
				{
					boot_info.bi_mac.HwMap |= HW_MAP_ADB_PB1;
				}
			}
		}
	}

	/* ASC */

	if (ASCBase != -1)
	{
		boot_info.bi_mac.HwMap |= HW_MAP_ASC;
		logical2physical(ASCBase, &boot_info.bi_mac.ASCBase);
	}

	/* SCSI 5380 */

	if (MacHasHardware(gestaltHasSCSI))
	{
		boot_info.bi_mac.HwMap |= HW_MAP_SCSI5380;
		logical2physical(SCSIBase, &boot_info.bi_mac.SCSIBase);
	}

	/* SCSI 5380 DMA */

	if (MacHasHardware(gestaltHasSCSIDMA))
	{
		boot_info.bi_mac.HwMap |= HW_MAP_SCSI5380DMA;
		logical2physical(SCSIBase, &boot_info.bi_mac.SCSIBase);
	}

	/* 5396, internal or external */

	if ( MacHasHardware(gestaltHasSCSI961) ||
	     MacHasHardware(gestaltHasSCSI962) )
	{
		boot_info.bi_mac.HwMap |= HW_MAP_SCSI5396;
		logical2physical(SCSIBase, &boot_info.bi_mac.SCSIBase);
	}

	/* ATA/IDE */

	if (HWCfgFlags & 0x0080)
	{
		if (PMgrBase != -1)
			boot_info.bi_mac.HwMap |= HW_MAP_IDE_PB;
		else
			boot_info.bi_mac.HwMap |= HW_MAP_IDE_QUADRA;
	}

	/* nubus */

	if (Gestalt('sltc', &boot_info.bi_mac.NubusMap) == noErr)
	{
		if (boot_info.bi_mac.NubusMap != 0)
		{
			boot_info.bi_mac.HwMap |= HW_MAP_NUBUS;
		}
	}

	/* SCC */
	
	if (MacHasHardware(gestaltHasSCCIOP))
		boot_info.bi_mac.HwMap |= HW_MAP_SCC_IOP;
#endif /* EXTENDED_HW_MAP */
}

static char *
add_v2_boot_record(char *dst, unsigned short tag,
		   unsigned short in_data_size, void *in_data)
{
	struct bi2_record *rec;

	rec = (struct bi2_record *)dst;
	rec->tag = tag;
	rec->size = in_data_size + sizeof(struct bi2_record);
	
	memcpy(rec->data, in_data, in_data_size);
	return (dst + sizeof(struct bi2_record) + in_data_size);
}

void
set_kernel_bootinfo(char *dst)
{
	unsigned long l1, l2, l3;

	/* machine type */

	l1 = boot_info.machtype;
	dst = add_v2_boot_record(dst, V2_BI_MACHTYPE, sizeof(l1), &l1);

	/* CPU type */

	switch(boot_info.cputype & CPU_MASK)
	{
		case CPU_68020:
			l1 = V2_CPU_68020;
			l2 = V2_MMU_68851;
			break;

		case CPU_68030:
			l1 = V2_CPU_68030;
			l2 = V2_MMU_68030;
			break;

		case CPU_68040:
			l1 = V2_CPU_68040;
			l2 = V2_MMU_68040;
			break;

		case CPU_68060:
			l1 = V2_CPU_68060;
			l2 = V2_MMU_68060;
			break;

		default:
			l1 = 0;
			l2 = 0;
			break;
	}

	switch(boot_info.cputype & FPU_MASK)
	{
		case FPU_68881:
			l3 = V2_FPU_68881;
			break;

		case FPU_68882:
			l3 = V2_FPU_68882;
			break;

		case FPU_68040:
			l3 = V2_FPU_68040;
			break;

		case FPU_68060:
			l3 = V2_FPU_68060;
			break;

		default:
			l3 = 0;
			break;
	}

	dst = add_v2_boot_record(dst, V2_BI_CPUTYPE, sizeof(l1), &l1);
	dst = add_v2_boot_record(dst, V2_BI_FPUTYPE, sizeof(l3), &l3);
	dst = add_v2_boot_record(dst, V2_BI_MMUTYPE, sizeof(l2), &l2);

	/* Memory chunk */

	for(l1 = 0; l1 < boot_info.num_memory; l1++)
	{
		dst = add_v2_boot_record(dst, V2_BI_MEMCHUNK, 
					 sizeof(struct mem_info), 
					 &boot_info.memory[l1]);
	}

	/* RAM disk */

	if (boot_info.ramdisk_size)
	{
		struct mem_info mi;

		mi.addr = boot_info.ramdisk_addr;
		mi.size = boot_info.ramdisk_size;

		dst = add_v2_boot_record(dst, V2_BI_RAMDISK, sizeof(mi), &mi);
	}

	dst = add_v2_boot_record(dst, V2_BI_COMMAND_LINE, 
				 strlen(boot_info.command_line) + 1, 
				 boot_info.command_line);

	l1 = boot_info.bi_mac.id;
	dst = add_v2_boot_record(dst, V2_BI_MAC_MODEL, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.videoaddr;
	dst = add_v2_boot_record(dst, V2_BI_MAC_VADDR, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.videodepth;
	dst = add_v2_boot_record(dst, V2_BI_MAC_VDEPTH, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.videorow;
	dst = add_v2_boot_record(dst, V2_BI_MAC_VROW, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.dimensions;
	dst = add_v2_boot_record(dst, V2_BI_MAC_VDIM, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.videological;
	dst = add_v2_boot_record(dst, V2_BI_MAC_VLOGICAL, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.scc;
	dst = add_v2_boot_record(dst, V2_BI_MAC_SCCBASE, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.boottime;
	dst = add_v2_boot_record(dst, V2_BI_MAC_BTIME, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.gmtbias;
	dst = add_v2_boot_record(dst, V2_BI_MAC_GMTBIAS, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.memsize;
	dst = add_v2_boot_record(dst, V2_BI_MAC_MEMSIZE, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.cpuid;
	dst = add_v2_boot_record(dst, V2_BI_MAC_CPUID, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.rombase;
	dst = add_v2_boot_record(dst, V2_BI_MAC_ROMBASE, sizeof(l1), &l1);

#if defined(EXTENDED_HW_MAP)
	/* VIA1 */

	if (boot_info.bi_mac.HwMap & HW_MAP_VIA1)
	{
		l1 = boot_info.bi_mac.VIA1Base;
		dst = add_v2_boot_record(dst, V2_BI_MAC_VIA1BASE, 
					      sizeof(l1), &l1);
	}

	/* VIA2 */

	if (boot_info.bi_mac.HwMap & HW_MAP_VIA2_VIA)
		l2 = V2_VIA2_VIA;
	else if (boot_info.bi_mac.HwMap & HW_MAP_VIA2_RBV)
		l2 = V2_VIA2_RBV;
	else if (boot_info.bi_mac.HwMap & HW_MAP_VIA2_OSS)
		l2 = V2_VIA2_OSS;

	if (boot_info.bi_mac.HwMap & 
			(HW_MAP_VIA2_VIA | HW_MAP_VIA2_RBV | HW_MAP_VIA2_OSS) )
	{
		l1 = boot_info.bi_mac.VIA2Base;

		dst = add_v2_boot_record(dst, V2_BI_MAC_VIA2BASE, 
					      sizeof(l1), &l1);
		dst = add_v2_boot_record(dst, V2_BI_MAC_VIA2TYPE, 
					      sizeof(l2), &l2);
	}

	/* ADB */

	if (boot_info.bi_mac.HwMap & HW_MAP_ADB_IOP)
		l1 = V2_ADB_IOP;
	else if (boot_info.bi_mac.HwMap & HW_MAP_ADB_IISI)
		l1 = V2_ADB_IISI;
	else if (boot_info.bi_mac.HwMap & HW_MAP_ADB_CUDA)
		l1 = V2_ADB_CUDA;
	else if (boot_info.bi_mac.HwMap & HW_MAP_ADB_PB1)
		l1 = V2_ADB_PB1;

	dst = add_v2_boot_record(dst, V2_BI_MAC_ADBTYPE, sizeof(l1), &l1);

	/* ASC */

	if (boot_info.bi_mac.HwMap & HW_MAP_ASC)
	{
		l1 = boot_info.bi_mac.ASCBase;
		dst = add_v2_boot_record(dst, V2_BI_MAC_ASCBASE, 
					      sizeof(l1), &l1);
	}

	/* SCSI 5380 */

	if (boot_info.bi_mac.HwMap & HW_MAP_SCSI5380)
	{
		l1 = boot_info.bi_mac.SCSIBase;

		dst = add_v2_boot_record(dst, V2_BI_MAC_SCSI5380, 
					      sizeof(l1), &l1);
	}

	/* SCSI 5380 DMA */

	if (boot_info.bi_mac.HwMap & HW_MAP_SCSI5380DMA)
	{
		l1 = boot_info.bi_mac.SCSIBase;

		dst = add_v2_boot_record(dst, V2_BI_MAC_SCSIDMA, 
					      sizeof(l1), &l1);
	}

	/* SCSI 5396, internal or external */

	if (boot_info.bi_mac.HwMap & HW_MAP_SCSI5396)
	{
		l1 = boot_info.bi_mac.SCSIBase;

		dst = add_v2_boot_record(dst, V2_BI_MAC_SCSI5396, 
					      sizeof(l1), &l1);
	}

	/* ATA/IDE */

	if (boot_info.bi_mac.HwMap & HW_MAP_IDE_PB)
	{
		l1 = V2_IDE_PB;
		dst = add_v2_boot_record(dst, V2_BI_MAC_IDETYPE, 
					      sizeof(l1), &l1);
		l2 = V2_IDE_BASE;
		dst = add_v2_boot_record(dst, V2_BI_MAC_IDEBASE, 
					      sizeof(l2), &l2);
	}
	else if (boot_info.bi_mac.HwMap & HW_MAP_IDE_QUADRA)
	{
		l1 = V2_IDE_QUADRA;
		dst = add_v2_boot_record(dst, V2_BI_MAC_IDETYPE, 
					      sizeof(l1), &l1);
		l2 = V2_IDE_BASE;
		dst = add_v2_boot_record(dst, V2_BI_MAC_IDEBASE, 
					      sizeof(l2), &l2);
	}

	/* NuBus */

	if (boot_info.bi_mac.HwMap & HW_MAP_NUBUS)
	{
		l2 = V2_NUBUS_NORMAL;
		dst = add_v2_boot_record(dst, V2_BI_MAC_NUBUS, sizeof(l2), &l2);
		l1 = boot_info.bi_mac.NubusMap;
		dst = add_v2_boot_record(dst, V2_BI_MAC_SLOTMASK, 
					      sizeof(l1), &l1);
	}

	/* SCC */

	if (boot_info.bi_mac.HwMap & HW_MAP_SCC_IOP)
		l1 = V2_SCC_NORMAL;
	else
		l1 = V2_SCC_IOP;
	dst = add_v2_boot_record(dst, V2_BI_MAC_SCCTYPE, sizeof(l1), &l1);
#endif	/* EXTENDED_HW_MAP */

	/* END */

	l1 = 0;
	dst = add_v2_boot_record(dst, V2_BI_LAST, 0, &l1);
}
