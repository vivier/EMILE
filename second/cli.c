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
			printf("%s", s + pos);
			l++;
			pos++;
			for (i = l; i > pos; i--)
				putchar('\b');
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
					strcpy(s + pos, s + pos + 1);
					printf("%s", s + pos);
					putchar(' ');
					for (i = l; i > pos; i--)
						putchar('\b');
					l--;
				}
				break;
			case 0x7f:	/* Delete */
				if (pos < l)
				{
					strcpy(s + pos, s + pos + 1);
					l--;
					printf("%s", s + pos);
					for (i = l; i > pos; i--)
						putchar('\b');
				}
				break;
		}
	}
}
