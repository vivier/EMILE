/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_ERRORS_H__
#define __MACOS_ERRORS_H__

#include <macos/traps.h>

enum {
	noErr = 0,
	qErr = -1,
	vTypErr = -2,
	corErr = -3,
	unimpErr = -4,
	SlpTypeErr = -5,
	seNoDB = -8,
	controlErr = -17,
	statusErr = -18,
	readErr = -19,
	writErr = -20,
	badUnitErr = -21,
	unitEmptyErr = -22,
	openErr = -23,
	closErr = -24,
	dRemovErr = -25,
	dInstErr = -26,
	paramErr = -50,
	userCanceledErr = -128,
	noHardwareErr = -200,
	notEnoughHardwareErr = -201,
	smNoMoresRsrcs = -344,
};

#ifdef __mc68000__
static inline void SysError(short errorCode)
{
	asm("move.w %0, %%d0\n"
		Trap(_SysError)
	    :: "g" (errorCode) : "%%d0", UNPRESERVED_REGS);
}
#endif /* __mc68000__ */
#endif /* __MACOS_ERRORS_H__ */
