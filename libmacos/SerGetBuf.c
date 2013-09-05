/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>

#include <macos/devices.h>
#include <macos/serial.h>

OSErr SerGetBuf(short refNum, long *count)
{
	int res;
	CntrlParam param;

	param.ioCompletion = 0;
	param.ioVRefNum = 0;
	param.ioCRefNum = refNum;
	param.csCode = kSERDInputCount;
	
	res = PBStatusSync((ParmBlkPtr)&param);

	*count = param.csParamLong;

	return res;
}
