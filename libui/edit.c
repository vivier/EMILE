/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#define __NO_INLINE__	/* to avoid inline putchar() */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "console.h"

#define LEFT_ARROW()	if (pos > 0)		\
			{			\
				putchar('\b');	\
				pos--;		\
			}

#define RIGHT_ARROW()	if (pos < l)		\
			{			\
				putchar(s[pos]);\
				pos++;		\
			}

#define DELETE()	if (pos < l)				\
			{					\
				strcpy(s + pos, s + pos + 1);	\
				l--;				\
				console_cursor_save();		\
				printf("%s", s + pos);		\
				putchar(' ');			\
				console_cursor_restore();	\
			}

void emile_edit(char *s, int length)
{
	int l = strlen(s);
	int pos = l;
	int c;
	int i;

	console_cursor_off();
	printf("%s", s);
	console_cursor_on();

	while ((c = console_getchar()) != '\r')
	{
retry:
		if (c == 0)
			continue;
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
			case '':
				while ((c = console_getchar()) == 0);
				if ( c != '[' )
					goto retry;
				while ((c = console_getchar()) == 0);
				switch(c)
				{
					case 'D':
						LEFT_ARROW();
						break;
					case 'C':
						RIGHT_ARROW();
						break;
					case '3':
						if (console_getchar() == '~')
							DELETE();
						break;
					default:
						goto retry;
				}
				break;
			case '':
				LEFT_ARROW();
				break;
			case '':
				RIGHT_ARROW();
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
				DELETE();
				break;
		}
	}
}
