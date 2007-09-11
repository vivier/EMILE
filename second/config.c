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
#include <libconfig.h>
#include "config.h"
#if defined(USE_CLI) && defined(__LINUX__)
#include "console.h"
#include "cli.h"
#endif
#include "arch.h"
#include "misc.h"
#include "bank.h"

#define MSG_STATE_0 console_set_cursor_position(5, 1); 		\
	printf("   Press 'b' or [RETURN] to boot or 'e' to edit\n");

#define MSG_STATE_1 console_set_cursor_position(3, 1);		\
	printf("   Press 'b' to boot, 'e' or [RETURN] to edit  \n"	\
	       "   Press 'd' to delete, 'n' to create          \n"	\
	       "   Press [ESC] to go back                      \n");

#define MSG_CLEAN console_set_cursor_position(3, 1);		\
	printf("                                               \n"	\
	       "                                               \n"	\
	       "                                               \n");

#define MAX_KERNELS		20
#define MAX_KERNEL_PARAMS	5

#define COMMAND_LINE_LENGTH	256
#define DEFAULT_TIMEOUT		5

static int8_t *open_config(emile_l2_header_t *info)
{
	stream_t *stream;
	struct stream_stat stat;
	int8_t *configuration;
	char property[COMMAND_LINE_LENGTH];
	int ret;

	if (config_get_property(info->configuration, 
				"configuration", property) != -1)
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

char* read_config_vga(emile_l2_header_t* info)
{
	int8_t *configuration;
	char property[64];
	int ret;

	configuration = open_config(info);
	ret = config_get_property(configuration, "vga", property);
	close_config(configuration);
	if (ret == -1)
		return NULL;

	return strdup(property);
}

int read_config_modem(emile_l2_header_t* info, int *bitrate, int *parity, int *datasize, int *stopbits)
{
	int8_t *configuration;
	char property[64];
	int ret;

	configuration = open_config(info);
	ret = config_get_property(configuration, "modem", property);
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
	ret = config_get_property(configuration, "printer", property);
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
	int current;
	int res;
#if defined(USE_CLI) && defined(__LINUX__)
	int l, c;
	emile_window_t win;
	emile_list_t list;
	int state;
#endif

	if (!EMILE_COMPAT(EMILE_07_SIGNATURE, info->signature))
	{
		printf("Bad header signature !\n");
		return -1;
	}

	configuration = open_config(info);

	if (config_get_property(configuration, "gestaltID", property) != -1)
	{
		machine_id = strtol(property, NULL, 0);
		printf("User forces gestalt ID to %ld\n", machine_id);
	}
	
	choice = 0;
	if (config_get_property(configuration, "default", property) != -1)
		choice = strtol(property, NULL, 0);
	
	timeout = DEFAULT_TIMEOUT;
	if (config_get_property(configuration, "timeout", property) != -1)
		timeout = strtol(property, NULL, 0);

	current = 0;
	for (index = 0; index < MAX_KERNELS; index++)
	{
		int prop;

		title[index] = NULL;

		res = config_get_property(configuration + current, 
					  "title", property);

		if (res == -1)
		{
			if (index)
				break;
			title[index] = strdup("Linux");
		}
		else
		{
			title[index] = strdup(property);
			if (title[index] == NULL)
			{
				close_config(configuration);
				return -1;
			}
			current += res;
			current = config_get_next_property(configuration,
							   current,
							   NULL, NULL);
		}
		prop = 0;
		for(i = 0; known_properties[i] != NULL; i++)
		{
			if (config_get_indexed_property(configuration, 
						(res == -1) ? NULL :"title", 
						title[index],
					 	known_properties[i], 
						property) == -1)
				continue;

			properties[index][prop] = 
					malloc(strlen(known_properties[i]) +
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
		prop_nb[index] = prop;
	}
	if (choice > index - 1)
		choice = index - 1;

#if defined(USE_CLI) && defined(__LINUX__)
	if (console_select(timeout))
		timeout = 0;

	console_get_size(&l, &c);
	console_clear();
#endif
	printf( "             EMILE v"VERSION
		" (c) 2004-2007 Laurent Vivier (%ld kB)\n",
		bank_mem_avail() / 1024);

#if defined(USE_CLI) && defined(__LINUX__)
	state = 0;

	win.l = 7;
	win.c = 4;
	win.h = l - 16;
	win.w = c - 8;

	MSG_STATE_0
	while(state != -1)
	{
		switch(state)
		{
		case 0:		/* select entry */
			list.item = title;
			list.nb = index;
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
				MSG_CLEAN
				break;
			case 'e':
			case 'E':
				list.current = 0;
				state = 1;
				MSG_STATE_1
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
				MSG_CLEAN
				break;
			case '\033':	/* ESC */
				list.current = choice;
				state = 0;
				MSG_CLEAN
				MSG_STATE_0
				break;
			case 'b':
			case 'B':
				MSG_CLEAN
				state = -1;
				break;
			case '\r':
			case 'e':
				MSG_CLEAN
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
			MSG_STATE_1
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

		id = config_read_word(properties[choice][i], &next);
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
