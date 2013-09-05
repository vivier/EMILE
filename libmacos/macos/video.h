/*
 *
 * (c) 2004-2008 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_VIDEO_H__
#define __MACOS_VIDEO_H__

#include <string.h>
#include <macos/files.h>
#include <macos/devices.h>
#include <macos/quickdraw.h>

/* control params */

/* SetEntries*/

typedef struct VDSetEntryRecord {
	ColorSpec *csTable;
	short csStart;
	short csCount;
} VDSetEntryRecord;
typedef VDSetEntryRecord *VDSetEntryPtr;

typedef struct VDGammaRecord {
	void *csGTable;
} VDGammaRecord;
typedef VDGammaRecord *VDGamRecPtr;

/* status params */

typedef struct VDFlagRec
{
        char flag;
} VDFlagRec;
typedef VDFlagRec *VDFlagPtr;

typedef struct VDParamBlock
{
	COMMON_PARAMS
        short ioRefNum;
        short csCode;
        void* csParam;
} VDParamBlock;
typedef VDParamBlock *VDParamBlockPtr;

typedef struct VDSwitchInfoRec {
	unsigned short csMode;
	unsigned long csData;
	unsigned short csPage;
	void*	csBaseAddr;
	unsigned long csReserved;
} VDSwitchInfoRec;

/* control codes */

enum {
	cscReset	= 0,
	cscKillIO	= 1,
	cscSetMode	= 2,
	cscSetEntries	= 3,
	cscSetGamma	= 4,
};

/* status codes */

enum {
	cscGetMode	= 2,
	cscGetEntries	= 3,
	cscGetGamma	= 8,
	cscGetCurMode	= 10,
};

/* status commands */

static inline OSErr GetCurrentMode(short refNum, VDSwitchInfoRec *hwMode)
{
	CntrlParam	param;

	memset(&param, 0, sizeof(param));
	memset(hwMode, 0, sizeof(*hwMode));

	param.csCode = cscGetCurMode;
	param.ioCRefNum = refNum;
	param.csParamPtr = hwMode;

	return PBStatusSync((ParmBlkPtr)&param);
}
#endif /* __MACOS_VIDEO_H__ */
