/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#define noErr	0

struct MachineLocation
{
	long latitude;
	long longitude;
	union
	{
		char dlsDelta;
		long gmtDelta;
	} u;
};
typedef struct MachineLocation MachineLocation;

/* ParamBlockRec Structure: "Inside Macintosh: Files", p. 2-87 */

enum {
	fsAtMark	= 0,
	fsFromStart	= 1,
	fsFromLEOF	= 2,
	fsFromMark	= 3
};

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

extern void glue_display_properties(unsigned long *base, 
				    unsigned long *row_bytes,
				    unsigned long *width, unsigned long *height,
				    unsigned long *depth, unsigned long *video);
extern int Gestalt(unsigned long selector, long * response);
extern void ReadLocation(MachineLocation * loc);
extern void* NewPtr(unsigned long byteCount);
extern void DisposePtr(void* ptr);
extern int PBReadSync(ParamBlockRec_t* paramBlock);
