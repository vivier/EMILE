/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <macos/types.h>

#include "misc.h"
#include "head.h"

extern void console_init(emile_l2_header_t* info);
extern inline int console_putchar(int c);
extern void console_putstring(const char *s);
#ifdef USE_CLI
extern int console_keypressed(int timeout);
extern int console_getchar(void);
extern void console_clear(void);
extern void console_cursor_on(void);
extern void console_cursor_off(void);
extern void console_cursor_save(void);
extern void console_cursor_restore(void);
extern void console_video_inverse(void);
extern void console_video_normal(void);
extern void console_set_cursor_position(int x, int y);
#endif

#endif
