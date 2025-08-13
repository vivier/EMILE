/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <macos/types.h>

#include "misc.h"
#include "head.h"

extern void console_init(void);
extern int console_putchar(int c);
extern void console_putstring(const char *s);
extern int wait_char;
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
extern void console_set_cursor_position(int l, int c);
extern int console_get_cursor_position(int* l, int* c);
extern int console_get_size(int *l, int *c);
extern int console_select(int timeout);
extern int console_status_request();
#endif

#endif
