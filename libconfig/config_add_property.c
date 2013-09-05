/*
 *
 * (c) 2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_add_property(int8_t* configuration, char* name, char* property)
{
	int index = strlen((char*)configuration);

	if (index > 0)
	{
		configuration[index] = '\n';
		index++;
	}
	sprintf((char*)configuration + index, 
		"%s %s", name, property);

	return index;
}
