/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

extern int console_putchar(int c);

int putchar (int c)
{
	return console_putchar(c);
}
