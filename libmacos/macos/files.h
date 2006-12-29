/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_FILES_H__
#define __MACOS_FILES_H__

#include <macos/traps.h>

enum {
	fsAtMark	= 0,
	fsFromStart	= 1,
	fsFromLEOF	= 2,
	fsFromMark	= 3
};

/* ParamBlockRec Structure: "Inside Macintosh: Files", p. 2-87 */

#define COMMON_PARAMS							\
	void*		qLink;		/* next queue entry */		\
	u_int16_t	qType;		/* queue type */		\
	u_int16_t	ioTrap;		/* routine trap */		\
	void*		ioCmdAddr;	/* routine address */		\
	void*		ioCompletion;	/* ptr to completion routine */	\
	u_int16_t	ioResult;	/* result code */		\
	unsigned char*	ioNamePtr;	/* pointer to pathname */	\
	int16_t		ioVRefNum;	/* volume specification */

typedef struct ParamBlockRec ParamBlockRec_t;
struct ParamBlockRec {
	COMMON_PARAMS
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
	COMMON_PARAMS
	int16_t			ioCRefNum;
	int16_t			csCode;
	int16_t			csParam[11];
};
typedef struct CntrlParam CntrlParam;
typedef CntrlParam* CntrlParamPtr;

struct VolumeParam {
	COMMON_PARAMS
	int32_t		filler2;
	int16_t		ioVolIndex;	/* Volume index number */
	u_int32_t	ioVCrDate;	/* Creation date/time */
	u_int32_t	ioVLsBkUp;	/* Last backup date/time */
	u_int16_t	ioVAtrb;	/* Volume attr */
	u_int16_t	ioVNmFls;	/* Number of files in directory */
	u_int16_t	ioVDirSt;	/* Start block of file directory */
	int16_t		ioVBlLn;	/* GetVolInfo:
					 * length of dir in blocks
					 */
	u_int16_t	ioVNmAlBlks;	/* For compatibilty:
					 * ioVNmAlBlks * ioVAlBlkSiz <= 2 GB
					 */
	u_int32_t	ioVAlBlkSiz;	/* For compatibilty:
					 * ioVAlBlkSiz is <= $0000FE00 (65,024)
					 */
	u_int32_t	ioVClpSiz;	/*GetVolInfo: bytes to allocate at a time*/
	u_int16_t	ioAlBlSt;	/* Starting disk(512-byte) block in block map */
	u_int32_t	ioVNxtFNum;	/* GetVolInfo: 
					 * next free file number
					 */
	u_int16_t	ioVFrBlk;	/* GetVolInfo:
					 * # free alloc blks for this volume
					 */
};
typedef struct VolumeParam	VolumeParam;
typedef VolumeParam*		VolumeParamPtr;
#define ASSERT_PBR(a)	if ( sizeof(ParamBlockRec_t) != 50 ) { a }

/* access permissions : Inside Macintosh: Devices
 * http://developer.apple.com/documentation/mac/Devices/
 */

enum {
	fsCurPerm	= 0,	/* retain current permission */
	fsRdPerm	= 1,	/* allow reads only */
	fsWrPerm	= 2,	/* allow writes only */
	fsRdWrPerm	= 3	/* allow reads and writes */
};

struct DrvQEl {
  void*            qLink;
  int16_t               qType;
  int16_t               dQDrive;
  int16_t               dQRefNum;
  int16_t               dQFSID;
  u_int16_t	dQDrvSz;
  u_int16_t	dQDrvSz2;
};
typedef struct DrvQEl                   DrvQEl;
typedef DrvQEl *                        DrvQElPtr;

#ifdef __mc68000__
static inline void FInitQueue(void)
{
	asm(Trap(_FInitQueue) ::: "%%d0", UNPRESERVED_REGS);
}

static inline OSErr PBGetVInfoSync(ParmBlkPtr paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_GetVolInfo)
	    : "=g" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}

static inline OSErr PBMountVol(ParmBlkPtr paramBlock)
{
	register OSErr ret asm("%%d0");

	asm("move.l %1, %%a0\n"
		Trap(_MountVol)
	    : "=g" (ret) : "g" (paramBlock) : UNPRESERVED_REGS);

	return ret;
}
#endif /* __mc68000__ */
#endif /* __MACOS_FILES_H__ */
