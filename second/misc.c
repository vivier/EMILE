/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <unistd.h>
#include <stdio.h>

#include "misc.h"

int strlen(const char* s)
{
	int len;

	if (!s) return 0;

	len = 0;
	while (*s++) len++;

	return len;
}

void* memset(void* s, int c, size_t n)
{
        int i;
        char *ss = (char*)s;

        for (i=0;i<n;i++) ss[i] = c;
        return s;
}

void* memcpy(void* __dest, __const void* __src,
                            size_t __n)
{
        int i;
        char *d = (char *)__dest, *s = (char *)__src;

        for (i=0;i<__n;i++) d[i] = s[i];
        return __dest;
}

char *strncpy (char *__restrict __dest,
                      __const char *__restrict __src, size_t __n)
{
	while ( (*__dest ++ = *__src++) && __n--);

	return __dest;
}

void error(char *x)
{
        printf("\n\n");
        printf(x);
        printf("\n\n -- System halted");

        while(1);       /* Halt */
}
