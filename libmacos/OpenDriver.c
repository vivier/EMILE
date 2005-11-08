/*
 * 
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <string.h>

#include <macos/devices.h>

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

	param.ioNamePtr = (unsigned char*)name;
	param.ioPermssn = fsCurPerm;

	err = PBOpenSync(&param);
	if (err != noErr)
		return err;

	*drvrRefNum = param.ioRefNum;

	return param.ioResult;
}
