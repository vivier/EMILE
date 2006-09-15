/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

#include "libemile.h"

int emile_second_get_property(char *configuration, char *name, char *property)
{
	int index = 0;
	char current_name[256];
	while (1)
	{
		index = emile_second_get_next_property(configuration, index, current_name, property);
		if (index == -1)
			break;
		if (strcmp(name, current_name) == 0)
			return 0;
	}
	property[0] = 0;
	return -1;
}
