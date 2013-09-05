/*
 *
 * (c) 2004 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

/*
 * WARNING: remember that m68k is big endian, like powerPC.
 *	    i386 is little-endian
 */

#ifndef _BOOTBLOCK_H
#define _BOOTBLOCK_H
static __attribute__((used)) char* bootblock_header = "$CVSHeader$";

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

#define COMMON_PARAMS                                                   \
	u_int32_t	qLink;	  /* next queue entry */	  \
	u_int16_t	qType;	  /* queue type */		\
	u_int16_t	ioTrap;	 /* routine trap */	      \
	u_int32_t	ioCmdAddr;      /* routine address */	   \
	u_int32_t	ioCompletion;   /* ptr to completion routine */ \
	u_int16_t	ioResult;       /* result code */	       \
	u_int32_t	ioNamePtr;      /* pointer to pathname */       \
	int16_t		ioVRefNum;      /* volume specification */

typedef struct ParamBlockRec ParamBlockRec_t;
struct ParamBlockRec {
	COMMON_PARAMS
	int16_t		ioRefNum;       /* file reference number */
	int8_t		ioVersNum;      /* version number */
	int8_t		ioPermssn;      /* read/write permission */
	u_int32_t	ioMisc;	 /* miscellaneaous */
	u_int32_t	ioBuffer;       /* data buffer */
	u_int32_t	ioReqCount;     /* requested number of bytes */
	u_int32_t	ioActCount;     /* actual number of bytes */
	u_int16_t	ioPosMode;      /* positioning mode and newline char */
	int32_t		ioPosOffset;    /* positionning offset */
} __attribute__((packed));

/* EMILE Boot block structure */

typedef struct eBootBlock eBootBlock_t;

struct eBootBlock {
	BootBlkHdr_t		boot_block_header;
	ParamBlockRec_t		second_param_block;
	u_int8_t		boot_code[1024 - sizeof(BootBlkHdr_t)
					       - sizeof(ParamBlockRec_t)];
} __attribute__((packed));

#define ASSERT_BB(a)	if ( sizeof(eBootBlock_t) != 1024 ) { a }

#endif
