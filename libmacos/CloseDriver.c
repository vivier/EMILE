/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

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
