/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#define __NO_INLINE__

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libstream.h>
#include <libui.h>
#include "config.h"
#if defined(USE_CLI) && defined(__LINUX__)
#include "console.h"
#include "cli.h"
#endif
#include "arch.h"
#include "misc.h"
#include "bank.h"

#define MAX_KERNELS		20
#define MAX_KERNEL_PARAMS	5

#define COMMAND_LINE_LENGTH	256
#define DEFAULT_TIMEOUT		5

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

static int get_next_property(int8_t *configuration, int index, char *name, char *property)
{
	char *next_word, *next_line;
	char *current_name, *current_property;

	next_line = (char*)configuration + index;
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

	return next_line - (char*)configuration;
}

static int get_indexed_property(int8_t *configuration, char *index_name, char *index_property, char *name, char *property)
{
	int found = (index_property == NULL); /* means not indexed */
	int index = 0;
	char current_name[256];

	while (1)
	{
		index = get_next_property(configuration, index, 
					  current_name, property);
		if (index == -1)
			return -1;
		if (found)
		{
			if (strcmp(name, current_name) == 0)
				return 0;
		}
		else
		{
			if ( (strcmp(index_name, current_name) == 0) && 
			     (strcmp(index_property, property) == 0) )
				found = 1;
		}
	}
	return -1;
}

static int get_property(int8_t *configuration, char *name, char *property)
{
	return get_indexed_property(configuration, NULL, NULL, name, property);
}

static int8_t *open_config(emile_l2_header_t *info)
{
	stream_t *stream;
	struct stream_stat stat;
	int8_t *configuration;
	char property[COMMAND_LINE_LENGTH];
	int ret;

	if (get_property(info->configuration, "configuration", property) == 0)
	{
		stream = stream_open(property);
		if (stream == NULL)
		{
			printf("ERROR: cannot open configuration file %s\n",
			       property);
			return NULL;
		}

		stream_fstat(stream, &stat);

		configuration = (int8_t*)malloc(stat.st_size);
		if (configuration == NULL)
		{
			printf("ERROR: cannot allocate %d bytes for "
			       "configuration file %s\n",
			       (int)stat.st_size, property);
			return NULL;
		}

		ret = stream_read(stream, configuration, stat.st_size);
		if (ret != stat.st_size)
		{
			printf("ERROR: cannot read %d bytes from "
			       "configuration file %s (%d)\n",
			       (int)stat.st_size, property, ret);
			return NULL;
		}

		stream_close(stream);
	}
	else
	{
		configuration = (int8_t*)malloc(info->conf_size);
		if (configuration == NULL)
		{
			printf("ERROR: cannot allocate %d bytes for "
			       "configuration file %s\n",
			       (int)stat.st_size, property);
			return NULL;
		}
		memcpy(configuration, info->configuration, info->conf_size);
	}

	return configuration;
}

static void close_config(int8_t *configuration)
{
	free(configuration);
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

int read_config_vga(emile_l2_header_t* info)
{
	int8_t *configuration;
	char property[64];
	int ret;

	configuration = open_config(info);
	ret = get_property(configuration, "vga", property);
	close_config(configuration);

	return ret;
}

int read_config_modem(emile_l2_header_t* info, int *bitrate, int *parity, int *datasize, int *stopbits)
{
	int8_t *configuration;
	char property[64];
	int ret;

	configuration = open_config(info);
	ret = get_property(configuration, "modem", property);
	if (ret == -1)
	{
		close_config(configuration);
		return -1;
	}

	decode_serial(property, bitrate, parity, datasize, stopbits);
	close_config(configuration);
	return 0;
}

int read_config_printer(emile_l2_header_t* info, int *bitrate, int *parity, int *datasize, int *stopbits)
{
	int8_t *configuration;
	char property[64];
	int ret;

	configuration = open_config(info);
	ret = get_property(configuration, "printer", property);
	if (ret == -1)
	{
		close_config(configuration);
		return -1;
	}

	decode_serial(property, bitrate, parity, datasize, stopbits);
	close_config(configuration);
	return 0;
}

int read_config(emile_l2_header_t* info, 
		char **kernel_path, char **command_line, char **ramdisk_path)
{ 
	char property[COMMAND_LINE_LENGTH];
	int8_t *configuration;
	int index;
	char* title[MAX_KERNELS];
	char* properties[MAX_KERNELS][MAX_KERNEL_PARAMS];
	int prop_nb[MAX_KERNELS];
	char *known_properties[] = { 
		"kernel", 
		"parameters", 
		"initrd", 
		NULL
	};
	int i;
	int choice;
	int timeout;
#if defined(USE_CLI) && defined(__LINUX__)
	emile_window_t win = { 7, 4, 8, 72 };
	emile_list_t list;
	int state;
	int res;
#endif

	printf( "             EMILE v"VERSION
		" (c) 2004-2007 Laurent Vivier (%ld kB)\n",
		bank_mem_avail() / 1024);

	if (!EMILE_COMPAT(EMILE_07_SIGNATURE, info->signature))
	{
		printf("Bad header signature !\n");
		return -1;
	}

	configuration = open_config(info);

	if (get_property(configuration, "gestaltID", property) == 0)
	{
		machine_id = strtol(property, NULL, 0);
		printf("User forces gestalt ID to %ld\n", machine_id);
	}
	
	choice = 0;
	if (get_property(configuration, "default", property) == 0)
		choice = strtol(property, NULL, 0);
	
	timeout = DEFAULT_TIMEOUT;
	if (get_property(configuration, "timeout", property) == 0)
		timeout = strtol(property, NULL, 0);

	for (index = 0; index < MAX_KERNELS; index++)
	{
		int prop;

		title[index] = NULL;

		if (get_property(configuration, "title", property) == 0)
		{
			title[index] = strdup(property);
			if (title[index] == NULL)
			{
				close_config(configuration);
				return -1;
			}
		}
		prop = 0;
		for(i = 0; known_properties[i] != NULL; i++)
		{
			if (get_indexed_property(configuration, "title", title[index],
					 	known_properties[i], property) == 0)
			{
				properties[index][prop] = malloc(strlen(known_properties[i]) +
								 strlen(property) + 2);
				if (properties[index][prop] == NULL)
				{
					close_config(configuration);
					return -1;
				}
				sprintf(properties[index][prop], "%s %s",
					known_properties[i], property);
				prop++;
			}
		}
		prop_nb[index] = prop;

		if (title[index] == NULL)	/* if no title, only one entry */
		{
			title[index] = strdup("Linux");
			break;
		}
	}
	if (choice > index)
		choice = index;

#if defined(USE_CLI) && defined(__LINUX__)
	state = 0;

	while(state != -1)
	{
		switch(state)
		{
		case 0:		/* select entry */
			list.item = title;
			list.nb = index + 1;
			list.current = choice;
			res = emile_scrolllist(&win, &list, timeout);
			choice = list.current;
			timeout = 0;
			switch(res)
			{
			case '\r':
			case 'b':
			case 'B':
				state = -1;
				break;
			case 'e':
			case 'E':
				list.current = 0;
				state = 1;
				break;
			}
			break;
		case 1: 	/* select parameter */
			list.item = properties[choice];
			list.nb = prop_nb[choice];
			res = emile_scrolllist(&win, &list, 0);
			switch(res)
			{
			case 'd':
			case 'D':
				free(properties[choice][list.current]);
				prop_nb[choice]--;
				for(i = list.current; i < prop_nb[choice]; i++)
					properties[choice][i] =
							properties[choice][i + 1];
				if (list.current >= prop_nb[choice])
					list.current--;
				break;
			case 'n':
			case 'N':
				properties[choice][prop_nb[choice]] = strdup("");
				list.current = prop_nb[choice];
				prop_nb[choice]++;
				state = 2;
				break;
			case '\033':	/* ESC */
				list.current = choice;
				state = 0;
				break;
			case 'b':
			case 'B':
				state = -1;
				break;
			case '\r':
			case 'e':
				state = 2;
				break;
			}
			break;
		case 2:		/* edit parameter */
			memset(property, 0, COMMAND_LINE_LENGTH);
			strncpy(property, properties[choice][list.current], COMMAND_LINE_LENGTH);
			free(properties[choice][list.current]);
			console_set_cursor_position(win.l + win.h + 2, 3);
			console_cursor_on();
			cli_edit(property, COMMAND_LINE_LENGTH);
			console_cursor_off();
			properties[choice][list.current] = strdup(property);
			console_set_cursor_position(win.l + win.h + 2, 3);
			memset(property, ' ', strlen(property));
			printf("%s", property);
			state = 1;
			break;
		}
	}
#endif

	*kernel_path = NULL;
	*command_line = NULL;
	*ramdisk_path = NULL;
	for (i = 0; i < prop_nb[choice]; i++)
	{
		char *id, *next;

		id = read_word(properties[choice][i], &next);
		*next = 0;
		next++;

		if (strcmp("kernel", id) == 0)
			*kernel_path = strdup(next);
		else if (strcmp("parameters", id) == 0)
			*command_line = strdup(next);
		else if (strcmp("initrd", id) == 0)
			*ramdisk_path = strdup(next);
	}

	for (index--; index >= 0; index--)
	{
		free(title[index]);

		for (i = 0; i < prop_nb[i]; i++)
			free(properties[index][i]);
	}

	close_config(configuration);
	return 0;
}
