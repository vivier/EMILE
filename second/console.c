/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "console.h"

#include "vga.h"

void
console_init(emile_l2_header_t* info)
{
	vga_init();
}

void
console_put(char c)
{
	vga_put(c);
}

void
console_print(char *s)
{
	while(*s)
		console_put(*(s++));
}
