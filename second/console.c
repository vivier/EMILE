/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

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

void
console_put(char c)
{
	if (vga_enabled)
		vga_put(c);
	serial_put(c);
}

void
console_print(char *s)
{
	while(*s)
		console_put(*(s++));
}
