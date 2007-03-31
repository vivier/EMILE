/*
 *
 * (c) 2004, 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __MISC_H__
#define __MISC_H__

extern unsigned char *c2pstring(char* s);
extern char *p2cstring(unsigned char* s);
extern void memdump(unsigned char* addr, unsigned long size);
extern void error(char *x) __attribute__ ((noreturn));

#endif /* __MISC_H__ */
