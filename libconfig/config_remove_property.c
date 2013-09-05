/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

int config_remove_property(int8_t *configuration, char *name)
{
	return config_remove_indexed_property(configuration, name, NULL, NULL);
}
