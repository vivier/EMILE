/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#define __NO_INLINE__	/* to avoid inline putchar() */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>

#include "libui.h"

#define LEFT_ARROW()	if (pos > 0)		\
			{			\
				putchar('\b');	\
				pos--;		\
			}

#define RIGHT_ARROW()	if (pos < l)		\
			{			\
				putchar(buf[pos]);\
				pos++;		\
			}

#define DELETE()	if (pos < l)				\
			{					\
				strcpy(buf + pos, buf + pos + 1);\
				l--;				\
				console_cursor_save();		\
				printf("%s", buf + pos);	\
				putchar(' ');			\
				console_cursor_restore();	\
			}

void emile_edit(char *line, int length)
{
	int l = strlen(line);
	int pos = l;
	int c;
	int i;
	int saved_wait = wait_char;
	char *buf;
	
	buf = malloc(length);
	memcpy(buf, line, length);

	console_cursor_off();
	printf("%s", buf);
	console_cursor_on();

	wait_char = 1;
	while (1)
	{
		console_keypressed(0);
		c = console_getchar();
		if (c == '\r')
		{
			strcpy(line, buf);
			break;
		}
retry:
		if ( (c > 0x1f) && (c < 0x7f) && (l < length - 1) )
		{
			for (i = l; i > pos; i--)
				buf[i] = buf[i - 1];
			buf[pos] = c;
			putchar(c);
			pos++;
			l++;
			console_cursor_save();
			printf("%s", buf + pos);
			console_cursor_restore();
		}
		else switch(c)
		{
			case '':
				c = console_getchar();
				if ( c != '[' )
					goto exit;
				c = console_getchar();
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
					strcpy(buf + pos, buf + pos + 1);
					console_cursor_save();
					printf("%s", buf + pos);
					putchar(' ');
					console_cursor_restore();
				}
				break;
			case 0x7f:	/* Delete */
				DELETE();
				break;
		}
	}
exit:
	wait_char = saved_wait;
	free(buf);
}
