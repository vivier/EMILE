/*
 * 
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>

#include <macos/lowmem.h>

#include "console.h"
#include "vga.h"

void
console_init()
{
	vga_init();
}

int console_putchar(int c)
{
	vga_put(c);

	return c;
}

void console_putstring(const char *s)
{
	while(*s)
                console_putchar(*(s++));
}
