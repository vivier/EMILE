
#define __NO_INLINE__

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libstream.h>
#include "config.h"
#if defined(USE_CLI) && defined(__LINUX__)
#include "console.h"
#include "cli.h"
#endif
#include "arch.h"
#include "misc.h"
#include "bank.h"

#define MAX_KERNELS 20

typedef struct emile_config {
        char* title;
        char* kernel;
        char* parameters;
        char* initrd;
} emile_config_t;

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
	emile_config_t config[MAX_KERNELS];
	char property[COMMAND_LINE_LENGTH];
	int8_t *configuration;
	int index;

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
	
	for (index = 0; index < MAX_KERNELS; index++)
	{
		memset(config + index, 0, sizeof(emile_config_t));

		if (get_property(configuration, "title", property) == 0)
		{
			config[index].title = strdup(property);
			if (config[index].title == NULL)
			{
				close_config(configuration);
				return -1;
			}
		}

		if (get_indexed_property(configuration, "title", config[index].title,
					 "kernel", property) == 0)
		{
			config[index].kernel = strdup(property);
			if (config[index].kernel == NULL)
			{
				close_config(configuration);
				return -1;
			}
		}
		else 
		{
			/* no kernel means end of list */

			break;
		}

		if (get_indexed_property(configuration, "title", config[index].title,
					 "parameters", property) == 0)
		{
			config[index].parameters = 
					(char*)malloc(COMMAND_LINE_LENGTH);
			if (config[index].parameters == NULL)
			{
				close_config(configuration);
				return -1;
			}
			memset(config[index].parameters, 0, COMMAND_LINE_LENGTH);
			strncpy(config[index].parameters, property, 
				COMMAND_LINE_LENGTH - 1);
		}

		if (get_indexed_property(configuration, "title", config[index].title,
					 "initrd", property) == 0)
		{
			config[index].initrd = strdup(property);
			if (config[index].initrd == NULL)
			{
				close_config(configuration);
				return -1;
			}
		}

		if (config[index].title == NULL)	/* if no title, only one entry */
			break;
	}

	printf("EMILE v"VERSION" (c) 2004-2007 Laurent Vivier (%ld kB)\n",
		bank_mem_avail() / 1024);

	*kernel_path = config[0].kernel;
	*command_line = config[0].parameters;
	*ramdisk_path = config[0].initrd;

	if (*kernel_path == NULL)
		error("No kernel path specified !");

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
	close_config(configuration);
	return 0;
}
