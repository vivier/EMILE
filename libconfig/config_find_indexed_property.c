/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_find_indexed_property(int8_t *configuration, char *index_name, char *index_property, char *name, char *property)
{
	int index;
	int last_index;
	char current_name[256];

	if (index_name == NULL)
		index = 0;
	else {
		index = config_find_entry(configuration, index_name, index_property);
		index = config_get_next_property(configuration, index, 
						 current_name, property);
	}

	while ((index != -1) && configuration[index])
	{
		last_index = index;
		index = config_get_next_property(configuration, index, 
						 current_name, property);
		if ((name != NULL) && (strcmp(name, current_name) == 0))
			return last_index;

		if ((index_name != NULL) && 
		    (strcmp(index_name, current_name)  == 0))
		{
		    	if (name == NULL)
				return last_index;
			break;
		}
	}
	if (property)
		property[0] = 0;	/* clear property */
	return -1;
}
