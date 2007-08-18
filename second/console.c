/*
 * 
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <macos/lowmem.h>

#include "vga.h"
#include "console.h"
#include "serial.h"
#include "keyboard.h"
#include "config.h"

static int vga_enabled = 0;

void
console_init(emile_l2_header_t* info)
{
	if (read_config_vga(info) == 0)
	{
		if (vga_init())
			vga_enabled = 0;
		else
			vga_enabled = 1;
	}
	serial_init(info);
}

int console_putchar(int c)
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

	while (!timeout || (Ticks < time))
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

void console_clear(void)
{
	printf("\033[2J");
}

void console_cursor_on(void)
{
	printf("\033[?25h");
}

void console_cursor_off(void)
{
	printf("\033[?25l");
}

void console_cursor_restore(void)
{
	printf("\0338");
}

void console_cursor_save(void)
{
	printf("\0337");
}

void console_video_inverse(void)
{
	printf("\033[7m");
}

void console_video_normal(void)
{
	printf("\033[27m");
}

void console_set_cursor_position(int l, int c)
{
	printf("\033[%d;%dH", l, c);
}

void console_select_charset(char c)
{
	printf("\033(%c", c);
}
#endif
