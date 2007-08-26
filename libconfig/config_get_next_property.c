/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libconfig.h"

static inline char *read_line(char *s)
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

char *config_read_word(char *line, char **next)
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

int config_get_next_property(char *configuration, int index, char *name, char *property)
{
	char *next_word, *next_line;
	char *current_name, *current_property;

	next_line = (char*)configuration + index;
	if (*next_line == 0)
		return -1;
	next_word = next_line;
	next_line = read_line(next_line);

	current_name = config_read_word(next_word, &next_word);
	if (name)
	{
		strncpy(name, current_name, next_word - current_name);
		name[next_word - current_name] = 0;
	}

	current_property = config_read_word(next_word, &next_word);
	if (property)
	{
		if (next_line - current_property != 0)
		{
			strncpy(property, current_property, 
				next_line - current_property);

			/* remove '\n' if needed */

			if (*(next_line - 1) == '\n')
				property[next_line - current_property - 1] = 0;
			else
				property[next_line - current_property] = 0;
		}
		else
			*property = 0;
	}

	return next_line - (char*)configuration;
}
