/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

extern void console_init();
extern void console_clear();

extern void console_put(char c);
extern void console_print(char *s);

extern unsigned long console_get_videobase();
extern unsigned long console_get_row_bytes();
extern unsigned long console_get_depth();
extern unsigned long console_get_width();
extern unsigned long console_get_height();
extern unsigned long console_get_video();

#endif
