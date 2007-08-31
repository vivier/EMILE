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
	int index ;
	
	index = config_find_indexed_property(configuration, 
					     index_name, index_property, 
					     name, property);
	return index;
}
