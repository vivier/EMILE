/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_VIDEO_H__
#define __MACOS_VIDEO_H__

#include <string.h>
#include <macos/files.h>
#include <macos/devices.h>

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

enum {
	cscGetCurMode	= 10,
};

static inline OSErr GetCurrentMode(short refNum, VDSwitchInfoRec *hwMode)
{
	CntrlParam	param;

	memset(&param, 0, sizeof(param));
	memset(hwMode, 0, sizeof(hwMode));

	param.csCode = cscGetCurMode;
	param.ioCRefNum = refNum;
	*((VDSwitchInfoRec **)&param.csParam[0]) = hwMode;

	return PBStatusSync((ParmBlkPtr)&param);
}
#endif /* __MACOS_VIDEO_H__ */
