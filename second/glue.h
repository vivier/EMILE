/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

typedef int16_t OSErr;

enum {
	noErr = 0
};

enum {
	false = 0,
	true = 1
};

struct MachineLocation
{
	int32_t latitude;
	int32_t longitude;
	union
	{
		int8_t dlsDelta;
		int32_t gmtDelta;
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

/* access permissions : Inside Macintosh: Devices
 * http://developer.apple.com/documentation/mac/Devices/
 */

enum {
	fsCurPerm	= 0,	/* retain current permission */
	fsRdPerm	= 1,	/* allow reads only */
	fsWrPerm	= 2,	/* allow writes only */
	fsRdWrPerm	= 3	/* allow reads and writes */
};

typedef struct ParamBlockRec ParamBlockRec_t;

struct ParamBlockRec {
	u_int32_t	qLink;		/* next queue entry */
	u_int16_t	qType;		/* queue type */
	u_int16_t	ioTrap;		/* routine trap */
	u_int32_t	ioCmdAddr;	/* routine address */
	u_int32_t	ioCompletion;	/* pointer to completion routine */
	u_int16_t	ioResult;	/* result code */
	u_int32_t	ioNamePtr;	/* pointer to pathname */
	int16_t		ioVRefNum;	/* volume specification */
	int16_t		ioRefNum;	/* file reference number */
	int8_t		ioVersNum;	/* version number */
	int8_t		ioPermssn;	/* read/write permission */
	u_int32_t	ioMisc;		/* miscellaneaous */
	u_int32_t	ioBuffer;	/* data buffer */
	u_int32_t	ioReqCount;	/* requested number of bytes */
	u_int32_t	ioActCount;	/* actual number of bytes */
	u_int16_t	ioPosMode;	/* positioning mode and newline char */
	int32_t		ioPosOffset;	/* positionning offset */
} __attribute__((packed));
typedef struct ParamBlockRec* ParmBlkPtr;

#define ASSERT_PBR(a)	if ( sizeof(ParamBlockRec_t) != 50 ) { a }

extern void glue_display_properties(unsigned long *base, 
				    unsigned long *row_bytes,
				    unsigned long *width, unsigned long *height,
				    unsigned long *depth, unsigned long *video);
extern OSErr Gestalt(unsigned long selector, long * response);
extern void ReadLocation(MachineLocation * loc);
extern void* NewPtr(unsigned long byteCount);
extern void DisposePtr(void* ptr);
extern OSErr PBReadSync(ParamBlockRec_t* paramBlock);
extern OSErr PBOpenSync(ParmBlkPtr paramBlock);
extern OSErr PBCloseSync(ParmBlkPtr paramBlock);
extern OSErr PBControlSync(ParmBlkPtr paramBlock);
