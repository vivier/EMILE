/*
 * 
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <macos/lowmem.h>

#include "vga.h"
#include "console.h"
#include "serial.h"
#include "keyboard.h"
#include "config.h"

enum {
	CONSOLE_ALL = 0,
	CONSOLE_VGA,
	CONSOLE_MODEM,
	CONSOLE_PRINTER,
};

static int selected_console;

void
console_init(emile_l2_header_t* info)
{
	selected_console = CONSOLE_ALL;

	vga_init(info);
	serial_init(info);
}

int console_putchar(int c)
{
	switch(selected_console)
	{
		case CONSOLE_ALL:
			vga_put(c);
			serial_put(SERIAL_MODEM_PORT, c);
			serial_put(SERIAL_PRINTER_PORT, c);
			break;
		case CONSOLE_VGA:
			vga_put(c);
			break;
		case CONSOLE_MODEM:
			serial_put(SERIAL_MODEM_PORT, c);
			break;
		case CONSOLE_PRINTER:
			serial_put(SERIAL_PRINTER_PORT, c);
			break;
	}

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
		if (serial_keypressed(SERIAL_MODEM_PORT))
			return CONSOLE_MODEM;

		if (serial_keypressed(SERIAL_PRINTER_PORT))
			return CONSOLE_PRINTER;

		if (vga_is_available() && keyboard_keypressed())
			return CONSOLE_VGA;
	}
	return 0;
}

int
console_select(int timeout)
{
	if (vga_is_available() +
	    serial_is_available(SERIAL_MODEM_PORT) +
	    serial_is_available(SERIAL_PRINTER_PORT) < 1)
	    return 0;

	printf("Press a key on this console to select it\n");

	if (console_keypressed(timeout * 60))
		return 1;

	if (selected_console == CONSOLE_ALL)
	{
		if (vga_is_available())
			selected_console = CONSOLE_VGA;
		else if (serial_is_available(SERIAL_MODEM_PORT))
			selected_console = CONSOLE_MODEM;
		else if (serial_is_available(SERIAL_PRINTER_PORT))
			selected_console = CONSOLE_PRINTER;
	}

	return 0;
}

int console_getchar()
{
	int c;

	switch(selected_console)
	{
		case CONSOLE_ALL:
			if (vga_is_available())
			{
				c = keyboard_getchar();
				if (c)
					return c;
			}
			c = serial_getchar(SERIAL_MODEM_PORT);
			if (c)
				return c;
			c = serial_getchar(SERIAL_PRINTER_PORT);
			if (c)
				return c;
			break;
		case CONSOLE_VGA:
			if (vga_is_available())
			{
				c = keyboard_getchar();
				if (c)
					return c;
			}
		case CONSOLE_MODEM:
			c = serial_getchar(SERIAL_MODEM_PORT);
			if (c)
				return c;
			break;
		case CONSOLE_PRINTER:
			c = serial_getchar(SERIAL_PRINTER_PORT);
			if (c)
				return c;
			break;
	}

	return 0;
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

void console_get_cursor_position(int *l, int *c)
{
	int car;
	char buf[16];
	int i;

	*l = *c = 0;

	while(console_getchar() != 0);
	printf("\033[6n");

	while ( (car = console_getchar()) && (car != '\033'));

	if (console_getchar() != '[')
		return;
	i = 0;
	while ( (car = console_getchar()) != ';')
		buf[i++] = car;
	buf[i] = 0;
	*l = strtol(buf, NULL, 10);
	i = 0;
	while ( (car = console_getchar()) != 'R')
		buf[i++] = car;
	buf[i] = 0;
	*c = strtol(buf, NULL, 10);
}

void console_select_charset(char c)
{
	printf("\033(%c", c);
}

void console_get_size(int *l, int *c)
{
	console_cursor_save();
	console_set_cursor_position(255, 255);
	console_get_cursor_position(l, c);
	console_cursor_restore();
}
#endif
