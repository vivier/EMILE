/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdarg.h>
#include <stdio.h>

int sprintf(char * s, const char * format, ...)
{
	va_list	params;
	int		r;
	
	va_start(params, format);
	r = vsprintf(s, format, params);
	va_end(params);
	
	return r;
}
