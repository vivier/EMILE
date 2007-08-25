/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

void config_set_property(char *configuration, char *name, char *property)
{
	config_set_indexed_property(configuration, NULL, NULL, name, property);
}
