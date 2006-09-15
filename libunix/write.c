/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <macos/devices.h>

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
