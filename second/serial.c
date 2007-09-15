/*
 * 
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <macos/types.h>
#include <macos/devices.h>
#include <macos/serial.h>

#include "misc.h"
#include "head.h"
#include "driver.h"
#include "config.h"
#include "serial.h"

static short out_refnum[SERIAL_PORT_NB];
#ifdef USE_CLI
static short in_refnum[SERIAL_PORT_NB];
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
 */

/*
 *
 * ".AOut" Serial port A (modem) output
 * ".AIn" Serial port A (modem) input
 * ".BOut" Serial port B (printer) output
 * ".BIn" Serial port B (printer) input
 *
 */

static int setserial(short refNum, unsigned int bitrate, unsigned int datasize, 
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

int serial_is_available(unsigned int port)
{
	if (port > SERIAL_PORT_NB)
		return 0;
	if (out_refnum[port] == -1)
		return 0;

	return 1;
}

void serial_put(unsigned int port, char c)
{
	if (!serial_is_available(port))
		return;

	if ( c == '\n' )
		write(out_refnum[port], "\n\r", 2);
	else
		write(out_refnum[port], &c, 1);
}

void serial_init(emile_l2_header_t* info)
{
	int res;
	int bitrate, parity, datasize, stopbits;
	int8_t *configuration;

	configuration = open_config(info);

	res = read_config_modem(configuration,
				&bitrate, &parity, &datasize, &stopbits);
	if (res == -1)
	{
		out_refnum[SERIAL_MODEM_PORT] = -1;
#ifdef USE_CLI
		in_refnum[SERIAL_MODEM_PORT] = -1;
#endif
	}
	else
	{
		res = OpenDriver(c2pstring(".AOut"), 
						&out_refnum[SERIAL_MODEM_PORT]);
		if (res != noErr) {
			printf("Cannot open modem output port (%d)\n", res);
		}
		else
		{
			res = setserial(out_refnum[SERIAL_MODEM_PORT], 
					bitrate,
					datasize,
					parity,
					stopbits);
			if (res != noErr) {
				printf("Cannot setup modem output port (%d)\n",
					res);
			}
		}
#ifdef USE_CLI
		res = OpenDriver(c2pstring(".AIn"), 
						&in_refnum[SERIAL_MODEM_PORT]);
		if (res != noErr) {
			printf("Cannot open modem input port (%d)\n", res);
		}
		else
		{
			res = setserial(in_refnum[SERIAL_MODEM_PORT], bitrate,
					datasize,
					parity,
					stopbits);
			if (res != noErr) {
				printf("Cannot setup modem input port (%d)\n",
					res);
			}
		}
#endif /* USE_CLI */
	}

	res = read_config_printer(configuration,
				  &bitrate, &parity, &datasize, &stopbits);
	if (res == -1)
	{
		out_refnum[SERIAL_PRINTER_PORT] = -1;
#ifdef USE_CLI
		in_refnum[SERIAL_PRINTER_PORT] = -1;
#endif
	}
	else
	{
		res = OpenDriver(c2pstring(".BOut"), 
					&in_refnum[SERIAL_PRINTER_PORT]);
		if (res != noErr) {
			printf("Cannot open printer output port (%d)\n", res);
		}
		else
		{
			res = setserial(in_refnum[SERIAL_PRINTER_PORT], 
					bitrate,
					datasize,
					parity,
					stopbits);
			if (res != noErr) {
				printf("Cannot setup printer output port (%d)\n"
					, res);
			}
		}
#ifdef USE_CLI
		res = OpenDriver(c2pstring(".BIn"), 
					&in_refnum[SERIAL_PRINTER_PORT]);
		if (res != noErr) {
			printf("Cannot open printer input port (%d)\n", res);
		}
		else
		{
			res = setserial(in_refnum[SERIAL_PRINTER_PORT],
					bitrate,
					datasize,
					parity,
					stopbits);
			if (res != noErr) {
				printf("Cannot setup printer input port (%d)\n"
					, res);
			}
		}
#endif /* USE_CLI */
	}
	close_config(configuration);
}

#ifdef USE_CLI
int serial_getchar(unsigned int port)
{
	int count;
	char c;

	if (!serial_is_available(port))
		return 0;

	count = read(in_refnum[port], &c, 1);
	if (count == 1)
		return c;

	return 0;
}

int serial_keypressed(unsigned int port)
{
	if (serial_getchar(port) != 0)
		return 1;

	return 0;
}
#endif
