/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <string.h>
#include <unistd.h>

#include "misc.h"
#include "glue.h"

/*
 * Technical Note TN1119 "Serial Port Apocrypha"
 *
 * from http://developer.apple.com/technotes/tn/tn1119.html
 *
 * Inside Macintosh, Devices, "Using the Serial Driver" :
 *
 * http://developer.apple.com/documentation/mac/Devices/Devices-315.html
 *
 * OpenDriver:
 *
 * http://developer.apple.com/documentation/mac/Devices/Devices-23.html
 *
 */

/*
 *
 * ".AOut" Serial port A (modem) output
 * ".AIn" Serial port A (modem) input
 * ".BOut" Serial port B (printer) output
 * ".BIn" Serial port B (printer) input
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

int setserial(short refNum, unsigned int bitrate, unsigned int datasize, 
			    int parity, int stopbits)
{
	CntrlParam param;
	short seropts;
	int res;

	switch(bitrate)
	{
		case 150:
			seropts = baud150;
			break;

		case 300:
			seropts = baud300;
			break;

		case 600:
			seropts = baud600;
			break;

		case 1200:
			seropts = baud1200;
			break;

		case 1800:
			seropts = baud1800;
			break;

		case 2400:
			seropts = baud2400;
			break;

		case 3600:
			seropts = baud3600;
			break;

		case 4800:
			seropts = baud3600;
			break;

		case 7200:
			seropts = baud7200;
			break;

		case 9600:
			seropts = baud9600;
			break;

		case 14400:
			seropts = baud14400;
			break;

		case 19200:
			seropts = baud19200;
			break;

		case 28800:
			seropts = baud28800;
			break;

		case 38400:
			seropts = baud38400;
			break;

		case 57600:
			seropts = baud57600;
			break;

		default:
			seropts = baud9600;
			break;
	}

	switch(datasize)
	{
		case 5:
			seropts |= data5;
			break;

		case 6:
			seropts |= data6;
			break;

		case 7:
			seropts |= data7;
			break;

		case 8:
			seropts |= data8;
			break;

		default:
			seropts |= data8;
			break;
	}

	switch(parity)
	{
		case 0:
			seropts |= noParity;
			break;

		case 1:
			seropts |= oddParity;
			break;

		case 2:
			seropts |= evenParity;
			break;
	}

	switch(stopbits)
	{
		case 0:
			seropts |= stop10;
			break;

		case 1:
			seropts |= stop15;
			break;

		case 2:
			seropts |= stop20;
			break;
	}

	param.csCode = kSERDConfiguration;
	param.csParam[0] = seropts;
	param.ioCRefNum = refNum;
	param.ioVRefNum = 0;
	param.ioCompletion = 0;
	res = PBControlSync((ParmBlkPtr)&param);

	return res;
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
