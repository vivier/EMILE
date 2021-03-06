/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_get_indexed_property(int8_t *configuration, char *index_name, char *index_property, char *name, char *property)
{
	int index ;
	
	index = config_find_indexed_property(configuration, 
					     index_name, index_property, 
					     name, property);
	return index;
}
