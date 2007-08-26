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
	int index;
	int len, len_new;

	len_new = strlen(name) + 1 + strlen(property) + 1;

	/* does this property exists in this indexed field ? */

	last_index = config_find_indexed_property(configuration,
						  index_name, index_property,
						  name, NULL);

	if (last_index == -1)
	{
		/* if not, does this indexed field exist ? */

		last_index = config_find_indexed_property(configuration,
						  index_name, index_property,
						  NULL, NULL);
		if (last_index == -1)
		{
			/* no, we add this property at the end */

			last_index = strlen(configuration);
			if (last_index > 0)
				last_index++; /* to insert a '\n' */
			index = last_index;
		}
		else
		{
			/* yes, we add this property at the end of this indexed field */

			last_index = config_get_next_property(configuration, last_index, NULL, NULL);
			if (last_index != -1)
				last_index = config_find_entry(configuration + last_index, index_name, NULL);
			if (last_index == -1)
				last_index = strlen(configuration);
			index = last_index;
		}
	}
	else
	{
		index = config_get_next_property(configuration, last_index, NULL, NULL);
		if (index == -1)
			index = strlen(configuration);
	}

	len = strlen(configuration + index);
	memmove(configuration + last_index + len_new,
	       configuration + index, len);

	if (last_index > 0)
		configuration[last_index - 1] = '\n';
	sprintf(configuration + last_index, 
		"%s %s", name, property);
	configuration[last_index + len_new - 1] = '\n';

	/* remove ending '\n' */

	len = strlen(configuration + last_index);
	if (configuration[last_index + len - 1] == '\n')
		len--;
	configuration[last_index + len] = 0;
}
