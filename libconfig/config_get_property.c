/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>

#include "libconfig.h"

int config_get_property(int8_t *configuration, char *name, char *property)
{
	return config_get_indexed_property(configuration, NULL, NULL, name, property);
}
