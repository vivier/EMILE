/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_find_indexed_property(char *configuration, char *index_name, char *index_property, char *name, char *property)
{
	int index;
	int last_index;
	char current_name[256];

	index = config_find_entry(configuration, index_name, index_property);
	if (index == -1)
		return -1;
	while (configuration[index])
	{
		last_index = index;
		index = config_get_next_property(configuration, index, 
						 current_name, property);
		if (index == -1)
			return -1;

		if (strcmp(name, current_name) == 0)
			return last_index;

		if ((index_name != NULL) && 
		    (strcmp(index_name, current_name)  == 0))
			return -1;
	}
	return -1;
}
