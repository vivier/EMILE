/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <string.h>
#include <sys/types.h>

#include <macos/devices.h>

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
