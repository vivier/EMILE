/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
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
