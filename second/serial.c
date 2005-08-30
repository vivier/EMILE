/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "misc.h"
#include "glue.h"
#include "head.h"

static short out_refnum0 = -1;
static short out_refnum1 = -1;
#ifdef USE_CLI
static short in_refnum0 = -1;
static short in_refnum1 = -1;
#endif

#if USE_BUFFER
#define BUFFER_LEN 80
static char buffer[256];
static int buff_len;
#endif

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

/*
 *
 * ".AOut" Serial port A (modem) output
 * ".AIn" Serial port A (modem) input
 * ".BOut" Serial port B (printer) output
 * ".BIn" Serial port B (printer) input
 *
 */

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

#ifdef USE_CLI
ssize_t read(int fd, void *buf, size_t count)
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
	res = PBReadSync(&param);
	if (res != noErr)
		return 0;
	
	return param.ioActCount;
}
#endif

void serial_put(char c)
{
#if USE_BUFFER
	buffer[buff_len++] = c;

	if ( c == '\n' )
	{
		/* add '\r' and flush buffer */

		buffer[buff_len++] = '\r';
		
		goto flush;
	}
	/* if buffer is full (BUFFER_LEN - 1), flush it
 	 * we take BUFFER_LEN - 1 to have enough room
	 * if we need to add '\r' on '\n'
	 */

	if (buff_len == BUFFER_LEN - 1)
		goto flush;

	return;
flush:
	if (out_refnum0 != -1)
		write(out_refnum0, buffer, buff_len);
	if (out_refnum1 != -1)
		write(out_refnum1, buffer, buff_len);
	buff_len = 0;
#else
	if ( c == '\n' )
	{
		if (out_refnum0 != -1)
			write(out_refnum0, "\n\r", 2);
		if (out_refnum1 != -1)
			write(out_refnum1, "\n\r", 2);
	}
	else
	{
		if (out_refnum0 != -1)
			write(out_refnum0, &c, 1);
		if (out_refnum1 != -1)
			write(out_refnum1, &c, 1);
	}
#endif
}

void serial_init(emile_l2_header_t* info)
{
	int res;

	if (info->console_mask & STDOUT_SERIAL0) {
		res = OpenDriver(c2pstring(".AOut"), &out_refnum0);
		if (res != noErr) {
			printf("Cannot open modem output port (%d)\n", res);
		}
		else
		{
			res = setserial(out_refnum0, info->serial0_bitrate,
					info->serial0_datasize,
					info->serial0_parity,
					info->serial0_stopbits);
			if (res != noErr) {
				printf("Cannot setup modem output port (%d)\n",
					res);
			}
		}
#ifdef USE_CLI
		res = OpenDriver(c2pstring(".AIn"), &in_refnum0);
		if (res != noErr) {
			printf("Cannot open modem input port (%d)\n", res);
		}
		else
		{
			res = setserial(in_refnum0, info->serial0_bitrate,
					info->serial0_datasize,
					info->serial0_parity,
					info->serial0_stopbits);
			if (res != noErr) {
				printf("Cannot setup modem input port (%d)\n",
					res);
			}
		}
#endif /* USE_CLI */
	}

	if (info->console_mask & STDOUT_SERIAL1) {
		res = OpenDriver(c2pstring(".BOut"), &out_refnum1);
		if (res != noErr) {
			printf("Cannot open printer output port (%d)\n", res);
		}
		else
		{
			res = setserial(out_refnum1, info->serial1_bitrate,
						info->serial1_datasize,
						info->serial1_parity,
						info->serial1_stopbits);
			if (res != noErr) {
				printf("Cannot setup printer output port (%d)\n"
					, res);
			}
		}
#ifdef USE_CLI
		res = OpenDriver(c2pstring(".BIn"), &in_refnum1);
		if (res != noErr) {
			printf("Cannot open printer input port (%d)\n", res);
		}
		else
		{
			res = setserial(in_refnum1, info->serial1_bitrate,
						info->serial1_datasize,
						info->serial1_parity,
						info->serial1_stopbits);
			if (res != noErr) {
				printf("Cannot setup printer input port (%d)\n"
					, res);
			}
		}
#endif /* USE_CLI */
	}

#if USE_BUFFER
	buff_len = 0;
#endif
}

#ifdef USE_CLI
void serial_cursor_save(void)
{
	serial_put('');
	serial_put('7');
}

void serial_cursor_restore(void)
{
	serial_put('');
	serial_put('8');
}

int serial_getchar(void)
{
	int count;
	char c;

	if (in_refnum0 != -1)
	{
		count = read(in_refnum0, &c, 1);
		if (count == 1)
			return c;
	}

	if (in_refnum1 != -1)
	{
		count = read(in_refnum1, &c, 1);
		if (count == 1)
			return c;
	}

	return -1;
}

int serial_keypressed()
{
	if (serial_getchar() != -1)
		return 1;

	return 0;
}
#endif
