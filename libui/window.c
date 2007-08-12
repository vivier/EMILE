/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include "libui.h"

void emile_window(emile_window_t *win)
{
	int i;
	int c = win->c - 1;
	int l = win->l - 1;
	int w = win->w + 2;
	int h = win->h + 2;

	console_select_charset('0');
	console_set_cursor_position(l, c);
	putchar('l');
	for (i = 1; i < w - 1; i++)
		putchar('q');
	putchar('k');
	for(i = 1; i < h - 1; i++)
	{
		console_set_cursor_position(l + i, c);
		putchar('x');
		console_set_cursor_position(l + i, c + w - 1);
		putchar('x');
	}
	console_set_cursor_position(l + h - 1, c);
	putchar('m');
	for (i = 1; i < w - 1; i++)
		putchar('q');
	putchar('j');
	console_select_charset('B');
}
