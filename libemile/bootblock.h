/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

/*
 * WARNING: remember that m68k is big endian, like powerPC.
 *	    i386 is little-endian
 */

#ifndef _BOOTBLOCK_H
#define _BOOTBLOCK_H
static __attribute__((used)) char* bootblock_header = "$CVSHeader$";
#include "../second/glue.h"

/* first level structure */

/* BootBlkHdr Structure: "Inside Macintosh: Files", p. 2-57 */

typedef struct BootBlkHdr BootBlkHdr_t;

struct BootBlkHdr {
	u_int16_t	ID;		/* boot blocks signature */
	u_int32_t	Entry;		/* entry point to bootcode */
	u_int16_t	Version;	/* boot blocks version number */
	u_int16_t	PageFlags;	/* used internally */
	u_int8_t	SysName[16];	/* System filename */
	u_int8_t	ShellName[16];	/* Finder filename */
	u_int8_t	Dbg1Name[16];	/* debugger filename */
	u_int8_t	Dbg2Name[16];	/* debugger filename */
	u_int8_t	ScreenName[16];	/* name of startup screen */
	u_int8_t	HelloName[16];	/* name of startup program */
	u_int8_t	ScrapName[16];	/* name of system scrap file */
	u_int16_t	CntFCBs;	/* number of FCBs to allocate */
	u_int16_t	CntEvts;	/* number of event queue elements */
	u_int32_t	Heap128K;	/* system heap size on 128K Mac */
	u_int32_t	Heap256K;	/* used internally */
	u_int32_t	SysHeapSize;	/* system heap size on all machines */
} __attribute__((packed));

#define ASSERT_BBH(a)	if ( sizeof(BootBlkHdr_t) != 138 ) { a }

/* EMILE Boot block structure */

typedef struct eBootBlock eBootBlock_t;

struct eBootBlock {
	BootBlkHdr_t		boot_block_header;
	ParamBlockRec_t		second_param_block;
	u_int8_t		boot_code[1024 - sizeof(BootBlkHdr_t)
					       - sizeof(ParamBlockRec_t)];
} __attribute__((packed));

#define ASSERT_BB(a)	if ( sizeof(eBootBlock_t) != 1024 ) { a }

#define FLOPPY_SECTOR_SIZE	512
#define FIRST_LEVEL_SIZE        (FLOPPY_SECTOR_SIZE * 2)

static inline unsigned long get_size(char* file)
{
	struct stat result;

	stat(file, &result);

	return (result.st_size + FLOPPY_SECTOR_SIZE - 1)
		/ FLOPPY_SECTOR_SIZE * FLOPPY_SECTOR_SIZE;
}
#endif
