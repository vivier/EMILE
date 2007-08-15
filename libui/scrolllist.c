/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>
#include "libui.h"

static int display_list(emile_window_t *win, emile_list_t *list, int base)
{
	int i, j;

	for (i = 0; i < win->h; i++)
	{
		console_set_cursor_position(win->l + i, win->c);
		if (i + base < list->nb)
		{
			char* current = list->item[i + base];
			int len = strlen(current);
			if ( (i + base) == list->current)
				console_video_inverse();
			for (j = 0; j < win->w; j++)
			{
				if (j < len)
					putchar(current[j]);
				else
					putchar(' ');
			}
			console_video_normal();
		}
		else
			for (j = 0; j < win->w; j++)
				putchar(' ');
	}

	return list->current;
}

int emile_scrolllist(emile_window_t *win, emile_list_t *list)
{
	char c;
	int base = 0;

	console_cursor_off();
	emile_window(win);
	display_list(win, list, base);

	while((c = getchar()) != -1)
	{
		if (c == '\033')
		{
			c = getchar();
			if (c != '[')
				continue;
			c = getchar();
			if ( (c == 'B') && (list->current < list->nb - 1) )
			{
				list->current++;
				if (list->current > win->h - 1)
					base = list->current - win->h + 1;
			}
			else if ( (c == 'A') && (list->current > 0) )
			{
				list->current--;
				if (list->current < base)
					base--;
			}

			display_list(win, list, base);
		}
		else
			break;
	}
	return c;
}
