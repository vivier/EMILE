
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

static char *read_line(char *s)
{
	int read = 0;
	while (*s && (*s != '\n'))
	{
		read++;
		s++;
	}
	if (*s == 0)
		return s;
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

static int get_next_property(char *configuration, int index, char *name, char *property)
{
	char *next_word, *next_line;
	char *current_name, *current_property;

	next_line = configuration + index;
	if (*next_line == 0)
		return -1;
	next_word = next_line;
	next_line = read_line(next_line);

	current_name = read_word(next_word, &next_word);
	strncpy(name, current_name, next_word - current_name);
	name[next_word - current_name] = 0;

	current_property = read_word(next_word, &next_word);
	if (next_line - current_property != 0)
	{
		strncpy(property, current_property, next_line - current_property);

		/* remove '\n' if needed */

		if (*(next_line - 1) == '\n')
			property[next_line - current_property - 1] = 0;
		else
			property[next_line - current_property] = 0;
	}
	else
		*property = 0;

	return next_line - configuration;
}

static int get_property(char *configuration, char *name, char *property)
{
	int index = 0;
	char current_name[256];
	while (1)
	{
		index = get_next_property(configuration, index, 
					  current_name, property);
		if (index == -1)
			break;
		if (strcmp(name, current_name) == 0)
			return 0;
	}
	return -1;
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
	char property[64];

	return get_property(conf, "vga", property);
}

int read_config_modem(char *conf, int *bitrate, int *parity, int *datasize, int *stopbits)
{
	char property[64];
	int ret;

	ret = get_property(conf, "modem", property);
	if (ret == -1)
		return -1;

	decode_serial(property, bitrate, parity, datasize, stopbits);
	return 0;
}

int read_config_printer(char *conf, int *bitrate, int *parity, int *datasize, int *stopbits)
{
	char property[64];
	int ret;

	ret = get_property(conf, "printer", property);
	if (ret == -1)
		return -1;

	decode_serial(property, bitrate, parity, datasize, stopbits);
	return 0;
}

int read_config(emile_l2_header_t* info, 
		char **kernel_path, char **command_line, char **ramdisk_path)
{
	char property[COMMAND_LINE_LENGTH];

	if (!EMILE_COMPAT(EMILE_06_SIGNATURE, info->signature))
	{
		printf("Bad header signature !\n");
		return -1;
	}

	*ramdisk_path = NULL;
	*kernel_path = NULL;
	*command_line = NULL;

	if (get_property(info->configuration, "kernel", property) == 0)
	{
		*kernel_path = strdup(property);
		if (*kernel_path == NULL)
			return -1;
	}

	if (get_property(info->configuration, "parameters", property) == 0)
	{
		*command_line = (char*)malloc(COMMAND_LINE_LENGTH);
		if (*command_line == NULL)
			return -1;
		memset(*command_line, 0, COMMAND_LINE_LENGTH);
		strncpy(*command_line, property, COMMAND_LINE_LENGTH - 1);
	}

	if (get_property(info->configuration, "initrd", property) == 0)
	{
		*ramdisk_path = strdup(property);
		if (*ramdisk_path == NULL)
			return -1;
	}

	if (get_property(info->configuration, "gestaltID", property) == 0)
	{
		machine_id = strtol(property, NULL, 0);
		printf("User forces gestalt ID to %ld\n", machine_id);
	}

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
