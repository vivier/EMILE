/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>

int default_unit = -1;

void stream_set_default(int unit)
{
	default_unit = unit;
}
