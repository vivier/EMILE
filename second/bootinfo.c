/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 *  a lot of parts from penguin booter
 *  based on bootstrap.c for Atari Linux booter, Copyright 1993 by Arjan Knor
 *
 */

#include <string.h>

#include "bank.h"
#include "memory.h"
#include "misc.h"
#include "glue.h"
#include "vga.h"
#include "lowmem.h"
#include "bootinfo.h"
#include "arch.h"

struct bootinfo boot_info;

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

#if defined(EXTENDED_HW_MAP)

static int
MacHasHardware(unsigned long gestaltBit)
{
	long	r;

	Gestalt(gestaltHardwareAttr, &r);

	return ( (r & (1 << gestaltBit)) != 0);
}
#endif /* EXTENDED_HW_MAP */

static void extractBanks(struct bootinfo *bi, memory_map_t *map)
{
	int i,j;

	for (i = 0; i < map->bank_number; i++)
	{
		bi->memory[i].addr = map->bank[i].physAddr;
		bi->memory[i].size = map->bank[i].size;
	}
	bi->num_memory = i;

	for (i = 0; i < bi->num_memory; i++)
	{
		for (j = 0; j < bi->num_memory; j++)
		{
			if (bi->memory[i].addr + bi->memory[i].size ==
							bi->memory[j].addr)
			{
				bi->memory[i].size += bi->memory[j].size;

				bi->num_memory--;
				bi->memory[j].addr = 
						bi->memory[bi->num_memory].addr;
				bi->memory[j].size = 
						bi->memory[bi->num_memory].size;
			}
		}
	}

	/* sort : bigger first */

	for (i = 0; i < bi->num_memory; i++)
	{
		for (j = i; j < bi->num_memory; j++)
		{
			if (bi->memory[i].size < bi->memory[j].size)
			{
				unsigned a, s;

				a = bi->memory[i].addr;
				s = bi->memory[i].size;

				bi->memory[i].addr = bi->memory[j].addr;
				bi->memory[i].size = bi->memory[j].size;

				bi->memory[j].addr = a;
				bi->memory[j].size = s;
			}
		}
	}
}

void bootinfo_init(char* command_line, 
		   char* ramdisk_start, unsigned long ramdisk_size)
{
	/* I'm a macintosh, I know, I'm sure */

	boot_info.machtype = MACH_MAC;

	/* WARNING: arch_init() must be called before ! */

	boot_info.bi_mac.id = machine_id;

	/* set ram size */

	boot_info.bi_mac.memsize = ram_size >> 20;	/* in mega-bytes */

	/* set processor type */

	switch (cpu_type)
	{
		case gestalt68000:
			break;

		case gestalt68010:
			break;

		case gestalt68020:
			if (mmu_type == gestalt68851)
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
	boot_info.bi_mac.cpuid = cpu_type - gestalt68020;

	/* Set the FPU info */

	switch (fpu_type)
	{
		case gestalt68881:
			boot_info.cputype |= FPU_68881;
			break;

		case gestalt68882:
			boot_info.cputype |= FPU_68882;
			break;

		case gestalt68040FPU:
			boot_info.cputype |= FPU_68040;
			break;

		case gestaltNoFPU:
		default:
			break;
	}

	/* memory structure */

	extractBanks(&boot_info, &memory_map);

	/* ramdisk info */

	boot_info.ramdisk_size = ramdisk_size;
#ifdef USE_MMU
	logical2physical((unsigned long)ramdisk_start, &boot_info.ramdisk_addr);
#else
	boot_info.ramdisk_addr = (unsigned long)ramdisk_start;
#endif

	/* command line */

	strncpy(boot_info.command_line, command_line, CL_SIZE);

	/* macintosh */

	/* video information */

	boot_info.bi_mac.videological = vga_get_video();
#ifdef USE_MMU
	logical2physical(vga_get_videobase(), &boot_info.bi_mac.videoaddr);
#else
	boot_info.bi_mac.videoaddr = vga_get_videobase();
#endif
	boot_info.bi_mac.videorow = vga_get_row_bytes();
	boot_info.bi_mac.videodepth = vga_get_depth();
	boot_info.bi_mac.dimensions = (vga_get_height() << 16) 
							| vga_get_width();

	/* booter version */

	boot_info.bi_mac.bootver = 108;

	/* boot time and time zone */

	boot_info.bi_mac.args = 0;
	boot_info.bi_mac.boottime = Time - 2082844800;

	boot_info.bi_mac.gmtbias = gmt_bias;

#ifdef USE_MMU
	logical2physical(SCCRd, &boot_info.bi_mac.scc_read);
	logical2physical(SCCWr, &boot_info.bi_mac.scc_write);
#else
	boot_info.bi_mac.scc_read = SCCRd;
	boot_info.bi_mac.scc_write = SCCWr;
#endif
	boot_info.bi_mac.timedbra = TimeDBRA;
	boot_info.bi_mac.adbdelay = TimeVIADB;
	boot_info.bi_mac.serialmf = 0;
	boot_info.bi_mac.serialhsk = 9600;
	boot_info.bi_mac.serialgpi = 9600;

	/* ROM base */

	boot_info.bi_mac.rombase = (unsigned long)ROMBase;

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

	if (machine_id == gestaltMacSE030) {
		boot_info.bi_mac.HwMap |= HW_MAP_NUBUS;
	} else {
		if (Gestalt('sltc', &boot_info.bi_mac.NubusMap) == noErr)
		{
			if (boot_info.bi_mac.NubusMap != 0)
			{
				boot_info.bi_mac.HwMap |= HW_MAP_NUBUS;
			}
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
	int aligned_size;	/* 68000 needs 4-byte aligned address */

	if ((((unsigned long)dst) & 0x3) != 0)
		error("add_v2_boot_record: not 4-byte aligned address");

	aligned_size = ((in_data_size + 3) / 4) * 4;

	rec = (struct bi2_record *)dst;
	rec->tag = tag;
	rec->size = aligned_size + sizeof(struct bi2_record);
	
	memcpy(rec->data, in_data, in_data_size);
	return (dst + sizeof(struct bi2_record) + aligned_size);
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

	l1 = boot_info.bi_mac.scc_read;
	dst = add_v2_boot_record(dst, V2_BI_MAC_SCCBASE_READ, sizeof(l1), &l1);

	l1 = boot_info.bi_mac.scc_write;
	dst = add_v2_boot_record(dst, V2_BI_MAC_SCCBASE_WRITE, sizeof(l1), &l1);

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
