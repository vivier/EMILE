/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_TYPES_H__
#define __MACOS_TYPES_H__

#ifndef _SYS_TYPES_H
typedef unsigned char u_int8_t;
typedef signed char int8_t;
typedef unsigned short u_int16_t;
typedef signed short int16_t;
typedef unsigned int u_int32_t;
typedef signed int int32_t;
#endif /* _SYS_TYPES_H */

typedef u_int8_t Byte;
typedef	int32_t	Fixed;
typedef int16_t OSErr;
typedef unsigned long FourCharCode;
typedef FourCharCode OSType;
typedef unsigned char Str255[256];
typedef const unsigned char * ConstStr255Param;
typedef unsigned char Str27[28];

typedef char*	Ptr;
typedef Ptr*	Handle;

enum {
	false = 0,
	true = 1 
};

struct Rect {
	int16_t	top;
	int16_t	left;
	int16_t	bottom;
	int16_t	right;
};
typedef struct Rect Rect;
typedef Rect* RectPtr;

struct Point {
	int16_t	v;
	int16_t	h;
};
typedef struct Point Point;
typedef Point* PointPtr;
#endif /* __MACOS_TYPES_H__ */
