/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

/*
 * WARNING: remember that m68k is big endian, like powerPC.
 *	    i386 is little-endian
 */

/* first level structure */

/* BootBlkHdr Structure: "Inside Macintosh: Files", p. 2-57 */

typedef struct BootBlkHdr BootBlkHdr_t;

struct BootBlkHdr {
	unsigned short	ID;		/* boot blocks signature */
	unsigned long	Entry;		/* entry point to bootcode */
	unsigned short	Version;	/* boot blocks version number */
	unsigned short	PageFlags;	/* used internally */
	unsigned char	SysName[16];	/* System filename */
	unsigned char	ShellName[16];	/* Finder filename */
	unsigned char	Dbg1Name[16];	/* debugger filename */
	unsigned char	Dbg2Name[16];	/* debugger filename */
	unsigned char	ScreenName[16];	/* name of startup screen */
	unsigned char	HelloName[16];	/* name of startup program */
	unsigned char	ScrapName[16];	/* name of system scrap file */
	unsigned short	CntFCBs;	/* number of FCBs to allocate */
	unsigned short	CntEvts;	/* number of event queue elements */
	unsigned long	Heap128K;	/* system heap size on 128K Mac */
	unsigned long	Heap256K;	/* used internally */
	unsigned long	SysHeapSize;	/* system heap size on all machines */
} __attribute__((packed));

#define ASSERT_BBH(a)	if ( sizeof(BootBlkHdr_t) != 138 ) { a }

/* ParamBlockRec Structure: "Inside Macintosh: Files", p. 2-87 */

typedef struct ParamBlockRec ParamBlockRec_t;

struct ParamBlockRec {
	unsigned long	qLink;		/* next queue entry */
	unsigned short	qType;		/* queue type */
	unsigned short	ioTrap;		/* routine trap */
	unsigned long	ioCmdAddr;	/* routine address */
	unsigned long	ioCompletion;	/* pointer to completion routine */
	unsigned short	ioResult;	/* result code */
	unsigned long	ioNamePtr;	/* pointer to pathname */
	signed short	ioVRefNum;	/* volume specification */
	signed short	ioRefNum;	/* file reference number */
	signed char	ioVersNum;	/* version number */
	signed char	ioPermssn;	/* read/write permission */
	unsigned long	ioMisc;		/* miscellaneaous */
	unsigned long	ioBuffer;	/* data buffer */
	unsigned long	ioReqCount;	/* requested number of bytes */
	unsigned long	ioActCount;	/* actual number of bytes */
	unsigned short	ioPosMode;	/* positioning mode and newline char */
	signed long	ioPosOffset;	/* positionning offset */
} __attribute__((packed));

#define ASSERT_PBR(a)	if ( sizeof(ParamBlockRec_t) != 50 ) { a }

/* EMILE Boot block structure */

typedef struct eBootBlock eBootBlock_t;

struct eBootBlock {
	BootBlkHdr_t		boot_block_header;
	ParamBlockRec_t		second_param_block;
	unsigned char		boot_code[1024 - sizeof(BootBlkHdr_t)
					       - sizeof(ParamBlockRec_t)];
} __attribute__((packed));

#define ASSERT_BB(a)	if ( sizeof(eBootBlock_t) != 1024 ) { a }

#define SECTOR_SIZE             512
#define FIRST_LEVEL_SIZE        (SECTOR_SIZE * 2)
