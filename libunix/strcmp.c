/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <string.h>

int strcmp(const char *__s1, const char *__s2)
{
	while (*__s1 && *__s1 == *__s2)
	{
		__s1++;
		__s2++;
	}

	return (*__s1 - *__s2);;
}
