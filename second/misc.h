/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __MISC_H__
#define __MISC_H__

typedef unsigned char u_int8_t;
typedef signed char int8_t;
typedef unsigned short u_int16_t;
typedef signed short int16_t;
typedef unsigned int u_int32_t;
typedef signed int int32_t;

extern unsigned char *c2pstring(char* s);
extern unsigned char *p2cstring(unsigned char* s);
extern void memdump(unsigned char* addr, unsigned long size);
extern void error(char *x) __attribute__ ((noreturn));

#endif /* __MISC_H__ */
