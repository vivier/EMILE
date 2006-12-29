/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_QUICKDRAW_H__
#define __MACOS_QUICKDRAW_H__
#include <macos/traps.h>


struct BitMap {
	void*	baseAddr;
	int16_t	rowBytes;
	Rect	bounds;
};
typedef struct BitMap BitMap;
typedef BitMap* BitMapPtr;
typedef BitMapPtr* BitMapHandle;

struct Bits16 {
	u_int16_t	elements[16];
};
typedef struct Bits16 Bits16;

struct Cursor {
	Bits16	data;
	Bits16	mask;
	Point	hotSpot;
};
typedef struct Cursor Cursor;
typedef Cursor* CursorPtr;
typedef CursorPtr* CursorHandle;

struct Pattern {
	u_int8_t	pat[8];
};
typedef struct Pattern Pattern;

typedef void* GrafPtr;
struct QDGlobals {
	int8_t		privates[76];
	int32_t	randSeed;
	BitMap	screenBits;
	Cursor	arrow;
	Pattern	dkGray;
	Pattern	ltGray;
	Pattern	gray;
	Pattern	black;
	Pattern	white;
	GrafPtr	thePort;
};
typedef struct QDGlobals QDGlobals;
typedef struct QDGlobals* QDGlobalsPtr;
typedef struct QDGlobalsPtr* QDGlobalsHandle;

#ifdef __mc68000__
static inline void InitGraf(void * port)
{
	asm("move.l %0, -(%%sp)\n"
		Trap(_InitGraf)
	    "	addq.l #4, %%sp"
	    :: "g" (port) : "%%d0", UNPRESERVED_REGS);
}
#endif /* __mc68000__ */
#endif /* __MACOS_QUICKDRAW_H__ */
