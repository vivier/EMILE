/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <macos/types.h>

extern void console_init(void);
extern inline int console_putchar(int c);
extern void console_putstring(const char *s);
#endif
