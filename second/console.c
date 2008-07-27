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
int wait_char;

void
console_init(void)
{
	selected_console = CONSOLE_ALL;
	wait_char = 0;
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

	/* wait user releases the previous key */

	while (serial_keypressed(SERIAL_MODEM_PORT) ||
	       serial_keypressed(SERIAL_PRINTER_PORT) ||
	       (vga_is_available() && keyboard_keypressed()))
	       ;

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
	int modem_available = 0;
	int printer_available = 0;
	int vga_available = 0;
	int available = 0;
	int ret;

	if (vga_is_available())
	{
		vga_available = 1;
		available++;
	}

	if (serial_is_available(SERIAL_MODEM_PORT))
	{
		selected_console = CONSOLE_MODEM;
		ret = console_status_request();
		if (ret == 0)
		{
			modem_available = 1;
			available++;
		}
	}

	if (serial_is_available(SERIAL_PRINTER_PORT))
	{
		selected_console = CONSOLE_PRINTER;
		ret = console_status_request();
		if (ret == 0)
		{
			printer_available = 1;
			available++;
		}
	}

	if (available > 1)
	{
		selected_console = CONSOLE_ALL;
		console_clear();
		console_set_cursor_position(1,1);
		printf("Please, press a key to select this console\n");

		selected_console = console_keypressed(timeout * 60);
		if (selected_console)
			return 0;
		timeout = 1;
	}

	if (vga_available)
		selected_console = CONSOLE_VGA;
	else if (modem_available)
		selected_console = CONSOLE_MODEM;
	else if (printer_available)
		selected_console = CONSOLE_PRINTER;

	return timeout;
}

int console_getchar()
{
	int c;
	long time = Ticks + wait_char * 60;

	c = 0;
	do {
		switch(selected_console)
		{
			case CONSOLE_ALL:
				if (vga_is_available())
				{
					c = keyboard_getchar();
					if (c)
						break;
				}
				c = serial_getchar(SERIAL_MODEM_PORT);
				if (c)
					break;
				c = serial_getchar(SERIAL_PRINTER_PORT);
				if (c)
					break;
				break;
			case CONSOLE_VGA:
				if (vga_is_available())
					c = keyboard_getchar();
				break;
			case CONSOLE_MODEM:
				c = serial_getchar(SERIAL_MODEM_PORT);
				break;
			case CONSOLE_PRINTER:
				c = serial_getchar(SERIAL_PRINTER_PORT);
				break;
		}
	} while ((c == 0) && (Ticks < time));

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

void console_flush()
{
	int saved_wait = wait_char;
	wait_char = 0;

	while(console_getchar() != 0);
	wait_char = saved_wait;
}

int console_get_cursor_position(int *l, int *c)
{
	int car;
	char buf[16];
	int i;
	int ret;
	int saved_wait = wait_char;

	ret = -1;
	*l = *c = 0;

	console_flush();
	printf("\033[6n");

	wait_char = 1;

	if (console_getchar() != '\033')
		goto out;

	if (console_getchar() != '[')
		goto out;

	ret = 0;
	i = 0;
	while ( (car = console_getchar()) && (car != ';'))
		buf[i++] = car;
	buf[i] = 0;
	*l = strtol(buf, NULL, 10);
	i = 0;
	while ( (car = console_getchar()) && (car != 'R'))
		buf[i++] = car;
	buf[i] = 0;
	*c = strtol(buf, NULL, 10);

out:
	wait_char = saved_wait;
	return ret;
}

void console_select_charset(char c)
{
	printf("\033(%c", c);
}

int console_get_size(int *l, int *c)
{
	int ret = 0;
	console_cursor_save();
	console_set_cursor_position(255, 255);
	ret = console_get_cursor_position(l, c);
	console_cursor_restore();
	return ret;
}

int console_status_request()
{
	int car;
	int status;
	int i;
	char buf[16];
	int saved_wait = wait_char;

	console_flush();
	printf("\033[5n");

	status = -1;
	wait_char = 1;
	car = console_getchar();
	if (car != '\033')
		goto out;

	car = console_getchar();
	if (car != '[')
		goto out;

	i = 0;
	while ( (car = console_getchar()) && (car != 'n'))
		buf[i++] = car;
	buf[i] = 0;
	if (car != 'n')
		goto out;

	status = strtol(buf, NULL, 10);

out:
	wait_char = saved_wait;
	return status;
}
#endif
