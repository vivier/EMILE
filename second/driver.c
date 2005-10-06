/*
 * 
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <unistd.h>
#include <string.h>

#include "misc.h"
#include "glue.h"

/*
 * OpenDriver:
 *
 * http://developer.apple.com/documentation/mac/Devices/Devices-23.html
 *
 */

OSErr OpenDriver(ConstStr255Param name, short *drvrRefNum)
{
	OSErr err;
	ParamBlockRec param;

	memset(&param, 0, sizeof(ParamBlockRec));

	param.ioNamePtr = (u_int32_t)name;
	param.ioPermssn = fsCurPerm;

	err = PBOpenSync(&param);
	if (err != noErr)
		return err;

	*drvrRefNum = param.ioRefNum;

	return param.ioResult;
}

OSErr CloseDriver(short refNum)
{
	OSErr err;
	ParamBlockRec param;

	memset(&param, 0, sizeof(ParamBlockRec));

	param.ioRefNum = refNum;

	err = PBCloseSync(&param);
	if (err != noErr)
		return err;

	return param.ioResult;
}

ssize_t write(int fd, const void *buf, size_t count)
{
	int res;
	ParamBlockRec param;

	param.ioCompletion = 0;
	param.ioVRefNum = 0;
	param.ioRefNum = fd;
	param.ioBuffer = (u_int32_t)buf;
	param.ioReqCount= count;
	param.ioPosMode = fsAtMark;
	param.ioPosOffset = 0;
	res = PBWriteSync(&param);
	if (res != noErr)
		return 0;
	
	return param.ioActCount;
}

#ifdef USE_CLI

static OSErr SerGetBuf(short refNum, long *count)
{
	int res;
	CntrlParam param;

	param.ioCompletion = 0;
	param.ioVRefNum = 0;
	param.ioCRefNum = refNum;
	param.csCode = kSERDInputCount;
	
	res = PBStatusSync((ParmBlkPtr)&param);

	*count = *(long*)&param.csParam;

	return res;
}

ssize_t read(int fd, void *buf, size_t count)
{
	int res;
	ParamBlockRec param;
	long available;

	res = SerGetBuf(fd, &available);
	if ( (res != noErr) || (available == 0) )
		return 0;

	param.ioCompletion = 0;
	param.ioVRefNum = 0;
	param.ioRefNum = fd;
	param.ioBuffer = (u_int32_t)buf;
	param.ioReqCount= count > available ? available : count;
	param.ioPosMode = fsAtMark;
	param.ioPosOffset = 0;
	res = PBReadSync(&param);
	if (res != noErr)
		return 0;
	return param.ioActCount;
}
#endif
