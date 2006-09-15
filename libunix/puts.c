/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

extern void console_putstring(const char *s);
extern int console_putchar(int c);

int puts(const char * s)
{
	console_putstring(s);
	console_putchar('\n');

	return -1;
}
