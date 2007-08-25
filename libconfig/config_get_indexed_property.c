/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_get_indexed_property(char *configuration, char *index_name, char *index_property, char *name, char *property)
{
	int found = (index_property == NULL); /* means not indexed */
	int index = 0;
	char current_name[256];

	while (1)
	{
		index = config_get_next_property(configuration, index, 
						 current_name, property);
		if (index == -1)
			return -1;
		if (found)
		{
			if ((index_name != NULL) && 
			    (strcmp(index_name, current_name)  == 0))
				return -1;
			if (strcmp(name, current_name) == 0)
				return 0;
		}
		else
		{
			if ( (strcmp(index_name, current_name) == 0) && 
			     (strcmp(index_property, property) == 0) )
				found = 1;
		}
	}
	return -1;
}
