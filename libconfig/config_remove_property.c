/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>

#include "libconfig.h"

void config_remove_property(char *configuration, char *name)
{
	config_remove_indexed_property(configuration, name, NULL, NULL);
}
