/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libemile.h"

void emile_second_remove_property(char *configuration, char *name)
{
	int index = 0;
	int current = 0;
	char current_name[256];
	char current_property[256];

	while (1)
	{
		index = emile_second_get_next_property(configuration, index, current_name, current_property);
		if (index == -1)
			break;
		if (strcmp(name, current_name) != 0)
		{
			memcpy(configuration + current, current_name, strlen(current_name));
			current += strlen(current_name);
			configuration[current++] = ' ';
			memcpy(configuration + current, current_property, strlen(current_property));
			current += strlen(current_property);
		}
	}
	if (configuration[current - 1] == '\n')
		configuration[current - 1] = 0;
}
