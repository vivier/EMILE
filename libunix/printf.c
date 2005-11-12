/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdarg.h>
#include <stdio.h>

extern void console_putstring(const char *s);

static char __printf_buffer[2048];

int
printf(const char * format, ...)
{
	va_list				args;
	int					len;

	va_start(args, format);
	len = vsprintf(__printf_buffer, format, args);
	va_end(args);

	if (len)
		console_putstring(__printf_buffer);

	return len;		
}
