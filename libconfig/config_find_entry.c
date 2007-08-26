/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_find_entry(char *configuration, char *name, char *property)
{
	int index = 0;
	int last_index;
	char current_name[256];
	char current_property[256];

	while (configuration[index])
	{
		last_index = index;
		index = config_get_next_property(configuration, index, 
						 current_name, current_property);
		if (index == -1)
			return -1;
		if ( (strcmp(name, current_name) == 0) && 
		     (strcmp(property, current_property) == 0) )
			return index;
		
	}
	return -1;
}
