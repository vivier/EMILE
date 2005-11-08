#include <sys/types.h>
#include <macos/devices.h>
#include <macos/serial.h>

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
