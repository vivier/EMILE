/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>

int strncmp(const char *__s1, const char *__s2, size_t __n)
{
	int tmp;
	while (__n--)
	{
		tmp = *__s1 - *__s2;
		if (tmp)
			return tmp;
		if (!*__s1)
			break;
		__s1++; __s2++;
	}
		
	return 0;
}
