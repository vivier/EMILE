/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libemile.h"

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

int emile_second_get_next_property(char *configuration, int index, char *name, char *property)
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
