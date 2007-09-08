/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "emile_config.h"

static int read_char(FILE* fd)
{
	int c;

	c = getc(fd);

	if (c == '#')
		while ( (c = getc(fd)) != '\n');
	
	return c;
}

static void skip_blank(FILE* fd)
{
	int c;

	do {
		c = read_char(fd);
	} while ( (c == ' ') || (c == '\t') );

	ungetc(c, fd);
}

static void skip_blank_line(FILE* fd)
{
	int c;

	do {
		skip_blank(fd);
		c = read_char(fd);
	} while (c == '\n');

	ungetc(c, fd);
}

static int read_word(FILE* fd, char* word)
{
	int index = 0;
	int c;

	skip_blank(fd);

	c = read_char(fd);
	if (c == -1)
		return -1;

	if (c == '\n')
	{
		word[index++] = c;
		word[index] = 0;
		return index;
	}
	while ( (c != ' ') && (c != '\t') && (c != '\n') && (c != -1) )
	{
		word[index++] = c;
		c = read_char(fd);
	}
	ungetc(c, fd);
	word[index] = 0;
	return index;
}

static int read_line(FILE* fd, char *name, char *value)
{
	int c;
	int index = 0;

	skip_blank_line(fd);

	if (read_word(fd, name) == -1)
		return -1;

	skip_blank(fd);

	c = read_char(fd);
	while ( (c != '\n') && (c != -1) )
	{
		value[index++] = c;
		c  = read_char(fd);
	}
	while ( (value[index-1] == ' ') || (value[index-1] == '\t') )
		index--;
	value[index] = 0;

	return 0;
}

static char* set_tag(char* string, int tag, int len, void* data)
{
	*string++ = tag;
	*string++ = len + 1;
	memcpy(string, data, len);
	string += len;

	*string = CONFIG_END;	/* mark end of string */

	return string;
}

static char* get_tag(char* string, int tag)
{
	while (*string != tag)
	{
		if (*string == CONFIG_END)
			return NULL;
		string++;
		string += *string;
	}
	
	return string + 2;
}

static int read_header(FILE* fd, char* header, int size)
{
	int offset = 0;
	char name[256];
	char value[1024];

	rewind(fd);

	while (read_line(fd, name, value) != -1)
	{
		if (strcmp("gestaltid", name) == 0)
		{
			int v = atoi(value);
			header = set_tag(header, CONFIG_GESTALTID, sizeof(int), &v);
		}
		else if (strcmp("vga", name) == 0)
		{
			header = set_tag(header, CONFIG_VGA, strlen(value) + 1, value);
		}
		else if (strcmp("modem", name) == 0)
		{
			header = set_tag(header, CONFIG_MODEM, strlen(value) + 1, value);
		}
		else if (strcmp("printer", name) == 0)
		{
			header = set_tag(header, CONFIG_PRINTER, strlen(value) + 1, value);
		}
		else if (strcmp("partition", name) == 0)
		{
			header = set_tag(header, CONFIG_PARTITION, strlen(value) + 1, value);
		}
		else if (strcmp("first_level", name) == 0)
		{
			header = set_tag(header, CONFIG_FIRST_LEVEL, strlen(value) + 1, value);
		}
		else if (strcmp("second_level", name) == 0)
		{
			header = set_tag(header, CONFIG_SECOND_LEVEL, strlen(value) + 1, value);
		}
		else if (strcmp("timeout", name) == 0)
		{
			int v = atoi(value);
			header = set_tag(header, CONFIG_TIMEOUT, sizeof(int), &v);
		}
		else if (strcmp("default", name) == 0)
		{
			int v = atoi(value);
			header = set_tag(header, CONFIG_DEFAULT, sizeof(int), &v);
		}
		else if (strcmp("title", name) == 0)
		{
			fseek(fd, offset, SEEK_SET);
			break;
		}
		else
		{
			fprintf(stderr, "ERROR: syntax error on word %s\n", name);
			return -1;
		}
		offset = ftell(fd);
	}
	return 0;
}

emile_config* emile_config_open(char* name)
{
	int ret;
	FILE* fd;
	emile_config* config;

	fd = fopen(name, "r");
	if (fd == NULL)
		return NULL;
	
	config = (emile_config*)malloc(sizeof(emile_config));
	if (config == NULL)
		goto close_file;

	memset(config, 0, sizeof(emile_config));

	config->fd = fd;
	config->header_size = 1024;
	config->header = (char*)malloc(config->header_size);
	if (config->header == NULL)
		goto free_config;
	config->current_size = 1024;
	config->current = (char*)malloc(config->current_size);
	if (config->current == NULL)
		goto free_header;

	ret = read_header(config->fd, config->header, config->header_size);
	if (ret == -1)
		goto free_current;

	return config;
free_current:
	free(config->current);
free_header:
	free(config->header);
free_config:
	free(config);
close_file:
	fclose(fd);
	return NULL;
}

void emile_config_close(emile_config* config)
{
	free(config->current);
	free(config->header);
	fclose(config->fd);
	free(config);
}

static int read_description(FILE* fd, char* desc, int size)
{
	int offset = 0;
	char name[256];
	char value[1024];
	int found = 0;

	while (read_line(fd, name, value) != -1)
	{
		if (strcmp("title", name) == 0)
		{
			if (found)
			{
				fseek(fd, offset, SEEK_SET);
				break;
			}
			found = 1;
			desc = set_tag(desc, CONFIG_TITLE, strlen(value) + 1, value);
		}
		else if (strcmp("kernel", name) == 0)
		{
			desc = set_tag(desc, CONFIG_KERNEL, strlen(value) + 1, value);
		}
		else if (strcmp("kernel_map", name) == 0)
		{
			desc = set_tag(desc, CONFIG_KERNEL_MAP, strlen(value) + 1, value);
		}
		else if (strcmp("args", name) == 0)
		{
			desc = set_tag(desc, CONFIG_ARGS, strlen(value) + 1, value);
		}
		else if (strcmp("initrd", name) == 0)
		{
			desc = set_tag(desc, CONFIG_INITRD, strlen(value) + 1, value);
		}
		else if (strcmp("initrd_map", name) == 0)
		{
			desc = set_tag(desc, CONFIG_INITRD_MAP, strlen(value) + 1, value);
		}
		else
		{
			fprintf(stderr, "ERROR: syntax error on word %s\n", name);
			return -1;
		}
		offset = ftell(fd);
	}
	return found ? 0 : -1;
}

int emile_config_read_next(emile_config* config)
{
	return read_description(config->fd, config->current, config->current_size);
}

int emile_config_read_first_entry(emile_config* config)
{
	int offset = 0;
	char name[256];
	char value[1024];

	rewind(config->fd);
	while (read_line(config->fd, name, value) != -1)
	{
		if (strcmp("title", name) == 0)
		{
			fseek(config->fd, offset, SEEK_SET);
			break;
		}
		offset = ftell(config->fd);
	}
	return read_description(config->fd, config->current, config->current_size);
}

int emile_config_get(emile_config* config, int tag, ...)
{
	int ret = -1;
	va_list arg;
	char **s;
	int *v;
	int *p;

	va_start(arg, tag);
	switch(tag)
	{
		case CONFIG_PARTITION:
	        case CONFIG_FIRST_LEVEL:
		case CONFIG_SECOND_LEVEL:
		case CONFIG_VGA:
		case CONFIG_MODEM:
		case CONFIG_PRINTER:
			s = va_arg(arg, char**);
			*s = get_tag(config->header, tag);
			ret = (*s == NULL) ? -1 : 0;
			break;
		case CONFIG_TIMEOUT:
		case CONFIG_DEFAULT:
			v = va_arg(arg, int*);
			p = (int*)get_tag(config->header, tag);
			if (p != NULL)
			{
				ret = 0;
				*v = *p;
			}
			break;
		case CONFIG_TITLE:
		case CONFIG_KERNEL:
		case CONFIG_ARGS:
		case CONFIG_INITRD:
			s = va_arg(arg, char**);
			*s = get_tag(config->current, tag);
			ret = (*s == NULL) ? -1 : 0;
			break;
	}
	va_end(arg);
	return ret;
}
