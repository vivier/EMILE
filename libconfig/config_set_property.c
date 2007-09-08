/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_set_property(int8_t *configuration, char *name, char *property)
{
	return config_set_indexed_property(configuration, NULL, NULL, 
					   name, property);
}
