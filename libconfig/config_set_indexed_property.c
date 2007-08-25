/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

void config_set_indexed_property(char *configuration, char *index_name, 
				 char *index_property, char *name, 
				 char *property)
{
	int found = (index_property == NULL); /* means not indexed */
	int index = 0;
	int last_index;
	char current_name[256];
	char current_property[256];

	while (configuration[index])
	{
		last_index = index;
		index = config_get_next_property(configuration, index, 
						 current_name, 
						 current_property);
		if (index == -1)
			return;
		if (found)
		{
			if (strcmp(name, current_name) != 0)
			{
				int len, len_new, len_old;

				len = strlen(configuration + index);
				len_old = index - last_index;
				len_new = strlen(name) + 1 + 
					  strlen(property) + 1;

				memcpy(configuration + last_index + len_new,
				       configuration + index, len);

				sprintf(configuration + last_index, 
					"%s %s", name, property);
				configuration[index + len_new] = '\n';
				if (configuration[index + len - 1] == '\n')
					len--;
				configuration[index + len] = 0;
				return;
			}
		} else {
			if ( (strcmp(index_name, current_name) == 0) &&
			     (strcmp(index_property, property) == 0) )
				found = 1;
		}
	}
}
