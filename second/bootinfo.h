/*
** asm/setup.h -- Definition of the Linux/m68k boot information structure
**
** Copyright 1992 by Greg Harp
**
** This file is subject to the terms and conditions of the GNU General Public
** License.  See the file COPYING in the main directory of this archive
** for more details.
**
** Created 09/29/92 by Greg Harp
**
** 5/2/94 Roman Hodek:
**   Added bi_atari part of the machine dependent union bi_un; for now it
**   contains just a model field to distinguish between TT and Falcon.
** 26/7/96 Roman Zippel:
**   Renamed to setup.h; added some useful macros to allow gcc some
**   optimizations if possible.
** 09/02/04 Laurent Vivier
**   Remove some parts and add it in EMILE
*/

#ifndef __ASSEMBLY__

struct mem_info {
	unsigned long addr;		/* physical address of memory chunk */
	unsigned long size;		/* length of memory chunk (in bytes) */
};

struct bi_Macintosh
{
	unsigned long videoaddr;
	unsigned long videorow;
	unsigned long videodepth;
	unsigned long dimensions;
	unsigned long args;
	unsigned long boottime;
	unsigned long gmtbias;
	unsigned long bootver;
	unsigned long videological;
	unsigned long scc_read;
	unsigned long scc_write;
	unsigned long id;
	unsigned long memsize;
	unsigned long serialmf;
	unsigned long serialhsk;
	unsigned long serialgpi;
	unsigned long printf;
	unsigned long printhsk;
	unsigned long printgpi;
	unsigned long cpuid;
	unsigned long rombase;
	unsigned long adbdelay;
	unsigned long timedbra;

#if defined(EXTENDED_HW_MAP)
#define HW_MAP_VIA1		0x00000001
#define HW_MAP_VIA2_VIA		0x00000002
#define HW_MAP_VIA2_RBV		0x00000004
#define HW_MAP_VIA2_OSS		0x00000008
#define HW_MAP_ADB_IOP		0x00000010
#define HW_MAP_ADB_IISI		0x00000020
#define HW_MAP_ADB_CUDA		0x00000040
#define HW_MAP_ADB_PB1		0x00000080
#define HW_MAP_ASC		0x00000100
#define HW_MAP_SCSI5380		0x00000200
#define HW_MAP_SCSI5380DMA	0x00000400
#define HW_MAP_SCSI5396		0x00000800
#define HW_MAP_IDE_PB		0x00001000
#define HW_MAP_IDE_QUADRA	0x00002000
#define HW_MAP_NUBUS		0x00004000
#define HW_MAP_SCC_IOP		0x00008000

	unsigned long HwMap;
	unsigned long VIA1Base;
	unsigned long VIA2Base;
	unsigned long ASCBase;
	unsigned long SCSIBase;
	unsigned long NubusMap;
#endif
};
	
#else

#define BI_videoaddr	BI_un
#define BI_videorow	BI_videoaddr+4
#define BI_videodepth	BI_videorow+4
#define BI_dimensions	BI_videodepth+4
#define BI_args		BI_dimensions+4

#endif

#define NUM_MEMINFO  4

#define MACH_AMIGA   1
#define MACH_ATARI   2
#define MACH_MAC     3

/*
 * CPU and FPU types
 */

#define CPUB_68020 0
#define CPUB_68030 1
#define CPUB_68040 2
#define CPUB_68060 3
#define FPUB_68881 5
#define FPUB_68882 6
#define FPUB_68040 7				/* Internal FPU */
#define FPUB_68060 8				/* Internal FPU */

#define CPU_68020    (1<<CPUB_68020)
#define CPU_68030    (1<<CPUB_68030)
#define CPU_68040    (1<<CPUB_68040)
#define CPU_68060    (1<<CPUB_68060)
#define CPU_MASK     (31)
#define FPU_68881    (1<<FPUB_68881)
#define FPU_68882    (1<<FPUB_68882)
#define FPU_68040    (1<<FPUB_68040)		/* Internal FPU */
#define FPU_68060    (1<<FPUB_68060)		/* Internal FPU */
#define FPU_MASK     (0xfe0)

#define CL_SIZE      (256)

#ifndef __ASSEMBLY__

struct bootinfo {
	unsigned long machtype;			/* machine type */
	unsigned long cputype;			/* system CPU & FPU */
	struct mem_info memory[NUM_MEMINFO];	/* memory description */
	unsigned long num_memory;				/* # of memory blocks found */
	unsigned long ramdisk_size;		/* ramdisk size in 1024 byte blocks */
	unsigned long ramdisk_addr;		/* address of the ram disk in mem */
	char command_line[CL_SIZE];		/* kernel command line parameters */
	union {
		struct bi_Macintosh bi_mac;	/* Mac specific information */
	} bi_un;
};
#define bi_mac	 bi_un.bi_mac

extern struct bootinfo boot_info;

#else	/* __ASSEMBLY__ */

BI_machtype	= 0
BI_cputype	= BI_machtype+4
BI_memory	= BI_cputype+4
BI_num_memory	= BI_memory+(MI_sizeof*NUM_MEMINFO)
BI_ramdisk_size	= BI_num_memory+4
BI_ramdisk_addr	= BI_ramdisk_size+4
BI_command_line	= BI_ramdisk_addr+4
BI_un		= BI_command_line+CL_SIZE

#endif /* __ASSEMBLY__ */


/*
 * Stuff for bootinfo interface versioning
 *
 * At the start of kernel code, a 'struct bootversion' is located. bootstrap
 * checks for a matching version of the interface before booting a kernel, to
 * avoid user confusion if kernel and bootstrap don't work together :-)
 *
 * If incompatible changes are made to the bootinfo interface, the major
 * number below should be stepped (and the minor reset to 0) for the
 * appropriate machine. If a change is backward-compatible, the minor should
 * be stepped. "Backwards-compatible" means that booting will work, but
 * certain features may not.
 */

#define BOOTINFOV_MAGIC			0x4249561A	/* 'BIV^Z' */
#define MK_BI_VERSION(major,minor)	(((major)<<16)+(minor))
#define BI_VERSION_MAJOR(v)		(((v) >> 16) & 0xffff)
#define BI_VERSION_MINOR(v)		((v) & 0xffff)

#ifndef __ASSEMBLY__

struct bootversion {
	unsigned short branch;
	unsigned long magic;
	struct {
		unsigned long machtype;
		unsigned long version;
	} machversions[1];
};

#endif /* __ASSEMBLY__ */

#define AMIGA_BOOTI_VERSION    MK_BI_VERSION( 1, 0 )
#define ATARI_BOOTI_VERSION    MK_BI_VERSION( 1, 0 )
#define MAC_BOOTI_VERSION      MK_BI_VERSION( 2, 0 )

/************************************************************************
 * Version 2 bootinfo structure and new defs
 */

    /*
     *  CPU, FPU and MMU types
     *
     *  Note: we may rely on the following equalities:
     *
     *      CPU_68020 == MMU_68851
     *      CPU_68030 == MMU_68030
     *      CPU_68040 == FPU_68040 == MMU_68040 (not strictly, think of 68LC040!)
     *      CPU_68060 == FPU_68060 == MMU_68060
     */

#define V2_CPUB_68020     0
#define V2_CPUB_68030     1
#define V2_CPUB_68040     2
#define V2_CPUB_68060     3

#define V2_CPU_68020      (1<<V2_CPUB_68020)
#define V2_CPU_68030      (1<<V2_CPUB_68030)
#define V2_CPU_68040      (1<<V2_CPUB_68040)
#define V2_CPU_68060      (1<<V2_CPUB_68060)

#define V2_FPUB_68881     0
#define V2_FPUB_68882     1
#define V2_FPUB_68040     2                       /* Internal FPU */
#define V2_FPUB_68060     3                       /* Internal FPU */
#define V2_FPUB_SUNFPA    4                       /* Sun-3 FPA */

#define V2_FPU_68881      (1<<V2_FPUB_68881)
#define V2_FPU_68882      (1<<V2_FPUB_68882)
#define V2_FPU_68040      (1<<V2_FPUB_68040)
#define V2_FPU_68060      (1<<V2_FPUB_68060)
#define V2_FPU_SUNFPA     (1<<V2_FPUB_SUNFPA)

#define V2_MMUB_68851     0
#define V2_MMUB_68030     1                       /* Internal MMU */
#define V2_MMUB_68040     2                       /* Internal MMU */
#define V2_MMUB_68060     3                       /* Internal MMU */
#define V2_MMUB_APOLLO    4                       /* Custom Apollo */
#define V2_MMUB_SUN3      5                       /* Custom Sun-3 */

#define V2_MMU_68851      (1<<V2_MMUB_68851)
#define V2_MMU_68030      (1<<V2_MMUB_68030)
#define V2_MMU_68040      (1<<V2_MMUB_68040)
#define V2_MMU_68060      (1<<V2_MMUB_68060)
#define V2_MMU_SUN3       (1<<V2_MMUB_SUN3)
#define V2_MMU_APOLLO     (1<<V2_MMUB_APOLLO)

struct bi2_record {
    unsigned short	tag;		/* tag ID */
    unsigned short	size;		/* size of record (in bytes) */
    unsigned char	data[0];		/* data */
};

struct bi2_record_list {
	struct bi2_record		*bi2_rec;
	struct bi2_record_list	*next;
};

    /*
     *  Tag Definitions
     *
     *  Machine independent tags start counting from 0x0000
     *  Machine dependent tags start counting from 0x8000
     */

#define V2_BI_LAST			0x0000	/* last record (sentinel) */
#define V2_BI_MACHTYPE		0x0001	/* machine type (u_long) */
#define V2_BI_CPUTYPE		0x0002	/* cpu type (u_long) */
#define V2_BI_FPUTYPE		0x0003	/* fpu type (u_long) */
#define V2_BI_MMUTYPE		0x0004	/* mmu type (u_long) */
#define V2_BI_MEMCHUNK		0x0005	/* memory chunk address and size */
					/* (struct mem_info) */
#define V2_BI_RAMDISK		0x0006	/* ramdisk address and size */
					/* (struct mem_info) */
#define V2_BI_COMMAND_LINE	0x0007	/* kernel command line parameters */
					/* (string) */

    /*
     *  Macintosh-specific tags
     */

#define V2_BI_MAC_MODEL		0x8000	/* Mac Gestalt ID (model type) */
#define V2_BI_MAC_VADDR		0x8001	/* Mac video base address */
#define V2_BI_MAC_VDEPTH	0x8002	/* Mac video depth */
#define V2_BI_MAC_VROW		0x8003	/* Mac video rowbytes */
#define V2_BI_MAC_VDIM		0x8004	/* Mac video dimensions */
#define V2_BI_MAC_VLOGICAL	0x8005	/* Mac video logical base */
#define V2_BI_MAC_SCCBASE_READ	0x8006	/* Mac SCC base address */
#define V2_BI_MAC_BTIME		0x8007	/* Mac boot time */
#define V2_BI_MAC_GMTBIAS	0x8008	/* Mac GMT timezone offset */
#define V2_BI_MAC_MEMSIZE	0x8009	/* Mac RAM size (sanity check) */
#define V2_BI_MAC_CPUID		0x800a	/* Mac CPU type (sanity check) */
#define V2_BI_MAC_ROMBASE	0x800b	/* Mac system ROM base address */ 
#define V2_BI_MAC_SCCBASE_WRITE	0x800c	/* Mac SCC base address */

    /*
     *  Macintosh hardware profile data - unused, see macintosh.h for 
     *  resonable type values 
     */

#define V2_BI_MAC_VIA1BASE	0x8010	/* Mac VIA1 base address (always present) */
#define V2_BI_MAC_VIA2BASE	0x8011	/* Mac VIA2 base address (type varies) */
#define V2_BI_MAC_VIA2TYPE	0x8012	/* Mac VIA2 type (VIA, RBV, OSS) */
#define V2_BI_MAC_ADBTYPE	0x8013	/* Mac ADB interface type */
#define V2_BI_MAC_ASCBASE	0x8014	/* Mac Apple Sound Chip base address */
#define V2_BI_MAC_SCSI5380	0x8015	/* Mac NCR 5380 SCSI (base address, multi) */
#define V2_BI_MAC_SCSIDMA	0x8016	/* Mac SCSI DMA (base address) */
#define V2_BI_MAC_SCSI5396	0x8017	/* Mac NCR 53C96 SCSI (base address, multi) */
#define V2_BI_MAC_IDETYPE	0x8018	/* Mac IDE interface type */
#define V2_BI_MAC_IDEBASE	0x8019	/* Mac IDE interface base address */
#define V2_BI_MAC_NUBUS		0x801a	/* Mac Nubus type (none, regular, pseudo) */
#define V2_BI_MAC_SLOTMASK	0x801b	/* Mac Nubus slots present */
#define V2_BI_MAC_SCCTYPE	0x801c	/* Mac SCC serial type (normal, IOP) */
#define V2_BI_MAC_ETHTYPE	0x801d	/* Mac builtin ethernet type (Sonic, MACE */
#define V2_BI_MAC_ETHBASE	0x801e	/* Mac builtin ethernet base address */
#define V2_BI_MAC_PMU		0x801f	/* Mac power managment / poweroff hardware */
#define V2_BI_MAC_IOP_SWIM	0x8020	/* Mac SWIM floppy IOP */
#define V2_BI_MAC_IOP_ADB	0x8021	/* Mac ADB IOP */

	/*
	 * Type definitions
	 */
#define V2_VIA2_VIA			1
#define V2_VIA2_RBV			2
#define V2_VIA2_OSS			3

#define V2_ADB_II			1
#define V2_ADB_IISI			2
#define V2_ADB_CUDA			3
#define V2_ADB_PB1			4
#define V2_ADB_PB2			5
#define V2_ADB_IOP			6

#define V2_IDE_BASE			0x50F1A000
#define V2_IDE_QUADRA		1
#define V2_IDE_PB			2

#define V2_NUBUS_NORMAL		1
#define V2_NUBUS_PSEUDO		2

#define V2_SCC_NORMAL		1
#define V2_SCC_IOP			4

/* End version 2 bootinfo
 ***********************************************************************/

extern void bootinfo_init(char* command_line, char* ramdisk_start, unsigned long ramdisk_size);
extern void set_kernel_bootinfo(char *dst);
