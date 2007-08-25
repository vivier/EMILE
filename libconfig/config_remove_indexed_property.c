/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

void config_remove_indexed_property(char *configuration, char *index_name,
				    char *index_property, char *name)
{
	int found = (index_property == NULL); /* means not indexed */
	int index = 0;
	int last_index;
	int current = 0;
	char current_name[256];
	char current_property[256];

	while (configuration[index])
	{
		last_index = index;
		index = config_get_next_property(configuration, 
						index, current_name, 
						current_property);
		if (index == -1)
			return;
		if (found)
		{
			if (strcmp(name, current_name) != 0)
			{
				int len;
				len = strlen(configuration + index);
				memcpy(configuration + last_index,
				       configuration + index, len);
				if (configuration[index + len - 1] == '\n')
					len--;
				configuration[index + len] = 0;
				return;
			}
		} else {
			if ( (strcmp(index_name, current_name) == 0) &&
			     (strcmp(index_property, current_property) == 0) )
				found = 1;
		}
	}
	if (configuration[current-1] == '\n')
		current--;
	configuration[current++] = 0;
}
