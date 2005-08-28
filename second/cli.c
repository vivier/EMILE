/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <string.h>

#include "console.h"

void cli_edit(char *s, int length)
{
	int l = strlen(s);
	int pos = l;
	int c = 1;
	int i;

	printf("%s", s);

	while ((c = console_getchar()) != '\r')
	{
		if ( (c > 0x1f) && (c < 0x7f) && (l < length - 1) )
		{
			for (i = l; i > pos; i--)
				s[i] = s[i - 1];
			s[pos] = c;
			putchar(c);
			pos++;
			l++;
			console_cursor_save();
			printf("%s", s + pos);
			console_cursor_restore();
		}
		else switch(c)
		{
			case '':
				if (pos > 0)
				{
					putchar('\b');
					pos--;
				}
				break;
			case '':
				if (pos < l)
				{
					putchar(s[pos]);
					pos++;
				}
				break;
			case '\b':	/* backspace */
				if (pos > 0)
				{
					putchar('\b');
					pos--;
					l--;
					strcpy(s + pos, s + pos + 1);
					console_cursor_save();
					printf("%s", s + pos);
					putchar(' ');
					console_cursor_restore();
				}
				break;
			case 0x7f:	/* Delete */
				if (pos < l)
				{
					strcpy(s + pos, s + pos + 1);
					l--;
					console_cursor_save();
					printf("%s", s + pos);
					putchar(' ');
					console_cursor_restore();
				}
				break;
		}
	}
}
