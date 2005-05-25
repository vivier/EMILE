/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "console.h"

#include "vga.h"
#include "serial.h"

static int vga_enabled = 0;

void
console_init(emile_l2_header_t* info)
{
	if (info->console_mask & STDOUT_VGA)
	{
		vga_init();
		vga_enabled = 1;
	}
	if ( info->console_mask & (STDOUT_SERIAL0 | STDOUT_SERIAL1) )
		serial_init(info);
}

inline int console_putchar(int c)
{
	if (vga_enabled)
		vga_put(c);
	serial_put(c);

	return c;
}

void console_putstring(const char *s)
{
	while(*s)
                console_putchar(*(s++));
}
