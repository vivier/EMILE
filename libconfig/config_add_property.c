/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_add_property(char* configuration, char* name, char* property)
{
	int index = strlen(configuration);

	if (index > 0)
	{
		configuration[index] = '\n';
		index++;
	}
	sprintf(configuration + index, 
		"%s %s", name, property);

	return index;
}
