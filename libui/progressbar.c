/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "libui.h"

emile_progressbar_t* emile_progressbar_create(emile_window_t *win, int max)
{
	int i;
	emile_progressbar_t *bar;

	if (max <= 0)
		return NULL;

	bar = (emile_progressbar_t*)malloc(sizeof(emile_progressbar_t));
	if (bar == NULL)
		return bar;

	bar->win = *win;
	bar->max = max;
	bar->current = 0;

	emile_window(&bar->win);

	console_set_cursor_position(bar->win.l, bar->win.c);
	console_select_charset('0');
	for (i = 0; i < bar->win.w; i++)
		putchar('a');
	console_select_charset('B');

	return bar;
}

void emile_progressbar_delete(emile_progressbar_t* bar)
{
	free(bar);
}

void emile_progressbar_value(emile_progressbar_t* bar, int value)
{
	int i;

	if ( (value < 0) || (value > bar->max) )
		return;

	console_select_charset('0');
	console_video_inverse();
	for (i = bar->current * bar->win.w / bar->max; 
		 i < value * bar->win.w / bar->max; i++)
	{
		console_set_cursor_position(bar->win.l, bar->win.c + i);
		putchar(' ');
	}
	console_video_normal();
	console_select_charset('B');

	bar->current = value;
}
