/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdlib.h>
#include <string.h>

char *strdup (__const char *__restrict __src)
{
	char *__dest;

	__dest = (char*)malloc(strlen(__src) + 1);
	if (__dest == NULL)
		return NULL;
	strcpy(__dest, __src);

	return __dest;
}
