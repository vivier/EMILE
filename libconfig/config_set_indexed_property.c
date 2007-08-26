/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

void config_set_indexed_property(char *configuration, 
				 char *index_name, char *index_property, 
				 char *name, char *property)
{
	int last_index;
	int len, len_new, len_old;

	len_new = strlen(name) + 1 + strlen(property) + 1;

	last_index = config_find_indexed_property(configuration,
						  index_name, index_property,
						  name, NULL);
	if (last_index != -1)
	{
		int index;

		index = config_get_next_property(configuration, last_index, 
						 NULL, NULL);

		len = strlen(configuration + index);
		len_old = index - last_index;
		len_new = strlen(name) + 1 + strlen(property) + 1;

		memcpy(configuration + last_index + len_new,
		       configuration + index, len);
	} else {
		last_index = config_find_indexed_property(configuration,
						  index_name, 
						  index_property,
						  index_name, NULL);

		len = strlen(configuration + last_index);
		memcpy(configuration + last_index + len_new,
		       configuration + last_index, len);
	}
	sprintf(configuration + last_index, 
		"%s %s", name, property);
	configuration[last_index + len_new] = '\n';
	len = strlen(configuration + last_index);
	if (configuration[last_index + len - 1] == '\n')
		len--;
	configuration[last_index + len] = 0;
}
