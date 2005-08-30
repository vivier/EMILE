/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "console.h"

#include "vga.h"
#include "serial.h"
#include "lowmem.h"
#include "keyboard.h"

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

#ifdef USE_CLI
int console_keypressed(int timeout)
{
	long time = Ticks + timeout;

	while (Ticks < time)
	{
		if (vga_enabled && keyboard_keypressed())
			return 1;

		if (serial_keypressed())
			return 1;
	}
	return 0;
}

int console_getchar()
{
	int c;
	if (vga_enabled)
	{
		c = keyboard_getchar();
		if (c)
			return c;
	}
	c = serial_getchar();
	return c;
}
void console_cursor_on(void)
{
	if (vga_enabled)
	{
		vga_cursor_on();
	}
}

void console_cursor_off(void)
{
	if (vga_enabled)
	{
		vga_cursor_off();
	}
}

void console_cursor_restore(void)
{
	if (vga_enabled)
	{
		vga_cursor_restore();
	}
	serial_cursor_restore();
}

void console_cursor_save(void)
{
	if (vga_enabled)
	{
		vga_cursor_save();
	}
	serial_cursor_save();
}
#endif
