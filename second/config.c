
#define __NO_INLINE__

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#if defined(USE_CLI) && defined(__LINUX__)
#include "console.h"
#include "cli.h"
#endif
#include "arch.h"

#define COMMAND_LINE_LENGTH 256
char parameters[COMMAND_LINE_LENGTH];

static char *read_line(char *s)
{
	int read = 0;
	while (*s && (*s != '\n'))
	{
		read++;
		s++;
	}
	return s + 1;
}

static char *read_word(char *line, char **next)
{
	char *word;

	while ( (*line == ' ') || (*line == '\t') || (*line == '\n') )
		line++;

	word = line;

	while ( *line && (*line != ' ') && (*line != '\t') && (*line != '\n') )
		line++;

	*next = line;

	return word;
}

static char *decode_serial(char* s, int *baudrate, int *parity, int *datasize, int *stopbits)
{
	*baudrate = strtol(s, &s, 0);
	switch(*s)
	{
		case 'n':
		case 'N':
			*parity = 0;
			break;
		case 'o':
		case 'O':
			*parity = 1;
			break;
		case 'e':
		case 'E':
			*parity = 2;
			break;
		default:
			*parity = -1;
			break;
	}
	s++;
	*datasize = strtol(s, &s, 0);
	if (*s != '+')
		return s;
	s++;
	*stopbits = strtol(s, &s, 0);

	return s;
}

int read_config_vga(char *conf)
{
	char *next_word, *next_line, *name, *property;
	int name_len;

	next_line = conf;

	while (*next_line)
	{
		next_word = next_line;
		next_line = read_line(next_line);
		name = read_word(next_word, &next_word);
		name_len = next_word - name;
		property = read_word(next_word, &next_word);

		if (strncmp(name, "vga", name_len) == 0)
		{
			return 0;
		}
	}
	return -1;
}

int read_config_modem(char *conf, int *bitrate, int *parity, int *datasize, int *stopbits)
{
	char *next_word, *next_line, *name, *property;
	int name_len;

	next_line = conf;

	while (*next_line)
	{
		next_word = next_line;
		next_line = read_line(next_line);
		name = read_word(next_word, &next_word);
		name_len = next_word - name;
		property = read_word(next_word, &next_word);

		if (strncmp(name, "modem", name_len) == 0)
		{
			decode_serial(property, bitrate, parity, datasize, stopbits);
			return 0;
		}
	}
	return -1;
}

int read_config_printer(char *conf, int *bitrate, int *parity, int *datasize, int *stopbits)
{
	char *next_word, *next_line, *name, *property;
	int name_len;
	next_line = conf;

	while (*next_line)
	{
		next_word = next_line;
		next_line = read_line(next_line);
		name = read_word(next_word, &next_word);
		name_len = next_word - name;
		property = read_word(next_word, &next_word);

		if (strncmp(name, "printer", name_len) == 0)
		{
			decode_serial(property, bitrate, parity, datasize, stopbits);
			return 0;
		}
	}
	return -1;
}

int read_config(emile_l2_header_t* info, 
		char **kernel_path, char **command_line, char **ramdisk_path)
{
	char *next_word, *next_line, *name, *property;

	if (!EMILE_COMPAT(EMILE_05_SIGNATURE, info->signature))
	{
		printf("Bad header signature !\n");
		return -1;
	}

	next_line = info->configuration;

	*ramdisk_path = NULL;
	*kernel_path = NULL;
	*command_line = NULL;
	while (*next_line)
	{
		next_word = next_line;
		next_line = read_line(next_line);
		*(next_line - 1) = 0;

		name = read_word(next_word, &next_word);
		*next_word++ = 0;

		property = read_word(next_word, &next_word);

		if (strcmp(name, "kernel") == 0)
		{
			*kernel_path = property;
		} else if (strcmp(name, "parameters") == 0) {
#if defined(USE_CLI) && defined(__LINUX__)
			*command_line = parameters;
			strncpy(parameters, property, COMMAND_LINE_LENGTH);
#else
			if (next_word != next_line)
				*command_line = property;
#endif
		}
		else if (strcmp(name, "initrd") == 0)
		{
			*ramdisk_path = property;
		}
		else if (strcmp(name, "gestaltID") == 0)
		{
			machine_id = strtol(property, NULL, 0);
			printf("User forces gestalt ID to %ld\n", machine_id);
		}
	}

	printf("kernel %s\n", *kernel_path);
	printf("initrd %s\n", *ramdisk_path);
#if defined(USE_CLI) && defined(__LINUX__)
	printf("command ");
	console_cursor_save();
	printf("%s", *command_line);
	console_cursor_on();
	if (console_keypressed(5 * 60))
	{
		console_cursor_restore();
		cli_edit(*command_line, COMMAND_LINE_LENGTH);
	}
	console_cursor_off();
	putchar('\n');
#else
#ifdef __LINUX__
	if (*command_line != NULL)
		printf("command %s\n", *command_line);
#endif
#endif
	return 0;
}
