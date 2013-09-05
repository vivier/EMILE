/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_SERIAL_H__
#define __MACOS_SERIAL_H__
enum {
	baud150		= 763,
	baud300		= 380,
	baud600		= 189,
	baud1200	= 94,
	baud1800	= 62,
	baud2400	= 46,
	baud3600	= 30,
	baud4800	= 22,
	baud7200	= 14,
	baud9600	= 10,
	baud14400	= 6,
	baud19200	= 4,
	baud28800	= 2,
	baud38400	= 1,
	baud57600	= 0
};

enum {
	data5		= 0,
	data6		= 2048,
	data7		= 1024,
	data8		= 3072
};

enum {
	noParity	= 0,
	oddParity	= 4096,
	evenParity	= 12288
};

enum {
	stop10		= 16384,
	stop15		= -32768L,
	stop20		= -16384
};

enum {
	 kSERDConfiguration	= 8,
	 kSERDInputBuffer	= 9,
	 kSERDSerHShake		= 10,
	 kSERDClearBreak	= 11,
	 kSERDSetBreak		= 12,
	 kSERDBaudRate		= 13,
	 kSERDHandshake		= 14,
	 kSERDClockMIDI		= 15,
	 kSERDMiscOptions	= 16,
	 kSERDAssertDTR		= 17,
	 kSERDNegateDTR		= 18,
	 kSERDSetPEChar		= 19,
	 kSERDSetPEAltChar	= 20,
	 kSERDSetXOffFlag	= 21,
	 kSERDClearXOffFlag	= 22,
	 kSERDSendXOn		= 23,
	 kSERDSendXOnOut	= 24,
	 kSERDSendXOff		= 25,
	 kSERDSendXOffOut	= 26,
	 kSERDResetChannel	= 27,
	 kSERDHandshakeRS232	= 28,
	 kSERDStickParity	= 29,
	 kSERDAssertRTS		= 30,
	 kSERDNegateRTS		= 31,
	 kSERD115KBaud		= 115,
	 kSERD230KBaud		= 230
};

enum {
	kSERDInputCount	= 2,	/* get available characters count (SerGetBuf) */
	kSERDStatus	= 8,	/* get status information (SerStatus) */
	kSERDVersion	= 9,	/* get driver version */
};
extern OSErr SerGetBuf(short refNum, long *count);
#endif /* __MACOS_SERIAL_H__ */
