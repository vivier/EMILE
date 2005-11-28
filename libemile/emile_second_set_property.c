/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libemile.h"
void emile_second_set_property(char *configuration, char *name, char *property)
{
	int index = 0;
	int current = 0;
	char current_name[256];
	char current_property[256];

	while (configuration[index])
	{
		index = emile_second_get_next_property(configuration, index, current_name, current_property);
		if (strcmp(name, current_name) != 0)
		{
			memcpy(configuration + current, current_name, strlen(current_name));
			current += strlen(current_name);
			configuration[current++] = ' ';
			memcpy(configuration + current, current_property, strlen(current_property));
			current += strlen(current_property);
			if (configuration[index])
				configuration[current++] = '\n';
			else
			{
				configuration[current++] = '\n';
				break;
			}
		}
	}
	sprintf(configuration + current, "%s %s", name, property);
}
