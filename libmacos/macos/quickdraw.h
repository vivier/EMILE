/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MACOS_QUICKDRAW_H__
#define __MACOS_QUICKDRAW_H__
#include <macos/types.h>
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

typedef struct ITab {
	long	iTabSeed;
	short	iTabRes;
	Byte	iTTable[1];
} ITab;
typedef ITab* ITabPtr;
typedef ITabPtr* ITabHandle;

typedef void** SProcHndl;

typedef void** CProcHndl;

typedef struct RGBColor {
	unsigned short	red;
	unsigned short green;
	unsigned short blue;
} RGBColor;

typedef struct ColorSpec {
	short	value;
	RGBColor	rgb;
} ColorSpec;
typedef ColorSpec* ColorSpecPtr;
typedef ColorSpec CSpecArray[1];

typedef struct ColorTable {
	long	ctSeed;
	short	ctFlags;
	short	ctSize;
	CSpecArray	ctTable;
} ColorTable;

typedef ColorTable* CTabPtr;
typedef CTabPtr* CTabHandle;

typedef struct PixMap {
	void*	baseAddr;
	short	rowBytes;
	Rect	bounds;
	short	pmVersion;
	short	packType;
	long	packSize;
	Fixed	hRes;
	Fixed	vRes;
	short	pixelType;
	short	pixelSize;
	short	cmpCount;
	short	cmpSize;
	OSType	pixelFormat;
	CTabHandle	pmTable;
	void*	pmExt;
} PixMap;

typedef PixMap* PixMapPtr;
typedef PixMapPtr* PixMapHandle;

typedef struct GDevice GDevice;
typedef GDevice* GDPtr;
typedef GDPtr* GDHandle;
struct GDevice {
	short		gdRefNum;
	short		gdID;
	short		gdType;
	ITabHandle	gdItable;
	short		gdResPref;
	SProcHndl	gdSearchProc;
	CProcHndl	gdCompProc;
	short		gdFlags;
	PixMapHandle		gdPMap;		/* PixMapHandle */
	long		gdRefCon;
	GDHandle	gdNextGD;
	Rect		gdRect;
	long		gdMode;
	short		gdCCBytes;
	short		gdCCDepth;
	Handle		gdCCXData;
	Handle		gdCCXMask;
	Handle		gdExt;
};

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
