/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

typedef int16_t OSErr;
typedef const unsigned char * ConstStr255Param;

enum {
	noErr = 0,
	qErr = -1,
	vTypErr = -2,
	corErr = -3,
	unimpErr = -4,
	SlpTypeErr = -5,
	seNoDB = -8,
	controlErr = -17,
	statusErr = -18,
	readErr = -19,
	writErr = -20,
	badUnitErr = -21,
	unitEmptyErr = -22,
	openErr = -23,
	closErr = -24,
	dRemovErr = -25,
	dInstErr = -26,
	paramErr = -50,
	userCanceledErr = -128,
	noHardwareErr = -200,
	notEnoughHardwareErr = -201,
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

enum {
	baud150		= 763,
	baud300		= 380,
	baud600		= 189,
	baud1200	= 94,
	baud1800	= 62,
	baud2400	= 46,
	baud3600	= 30,
	baud4800	= 22,
	baud7200	= 14,
	baud9600	= 10,
	baud14400	= 6,
	baud19200	= 4,
	baud28800	= 2,
	baud38400	= 1,
	baud57600	= 0
};

enum {
	data5		= 0,
	data6		= 2048,
	data7		= 1024,
	data8		= 3072
};

enum {
	noParity	= 0,
	oddParity	= 4096,
	evenParity	= 12288
};

enum {
	stop10		= 16384,
	stop15		= -32768L,
	stop20		= -16384
};

enum {
	 kSERDConfiguration	= 8,
	 kSERDInputBuffer	= 9,
	 kSERDSerHShake		= 10,
	 kSERDClearBreak	= 11,
	 kSERDSetBreak		= 12,
	 kSERDBaudRate		= 13,
	 kSERDHandshake		= 14,
	 kSERDClockMIDI		= 15,
	 kSERDMiscOptions	= 16,
	 kSERDAssertDTR		= 17,
	 kSERDNegateDTR		= 18,
	 kSERDSetPEChar		= 19,
	 kSERDSetPEAltChar	= 20,
	 kSERDSetXOffFlag	= 21,
	 kSERDClearXOffFlag	= 22,
	 kSERDSendXOn		= 23,
	 kSERDSendXOnOut	= 24,
	 kSERDSendXOff		= 25,
	 kSERDSendXOffOut	= 26,
	 kSERDResetChannel	= 27,
	 kSERDHandshakeRS232	= 28,
	 kSERDStickParity	= 29,
	 kSERDAssertRTS		= 30,
	 kSERDNegateRTS		= 31,
	 kSERD115KBaud		= 115,
	 kSERD230KBaud		= 230
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
typedef struct ParamBlockRec ParamBlockRec;
typedef struct ParamBlockRec* ParmBlkPtr;

struct CntrlParam {
	u_int32_t		qLink;
	int16_t			qType;
	int16_t			ioTrap;
	u_int32_t		ioCmdAddr;
	u_int32_t		ioCompletion;
	volatile OSErr		ioResult;
	char*			ioNamePtr;
	int16_t			ioVRefNum;
	int16_t			ioCRefNum;
	int16_t			csCode;
	int16_t			csParam[11];
};
typedef struct CntrlParam CntrlParam;
typedef CntrlParam* CntrlParamPtr;

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
extern OSErr PBWriteSync(ParamBlockRec_t* paramBlock);
extern OSErr PBOpenSync(ParmBlkPtr paramBlock);
extern OSErr PBCloseSync(ParmBlkPtr paramBlock);
extern OSErr PBControlSync(ParmBlkPtr paramBlock);
