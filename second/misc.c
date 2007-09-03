/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#include "misc.h"
#include "console.h"

unsigned char *c2pstring(char* s)
{
	int len = strlen(s);
	int i;

	for (i = len; i > 0; i--)
		s[i] = s[i - 1];
	s[0] = len;

	return (unsigned char*)s;
}

static char buffer[256];
char *p2cstring(unsigned char* s)
{

	memcpy(buffer, s + 1, s[0]);
	buffer[(int)s[0]] = (char)0;

	return buffer;
}

void error(char *x)
{
        console_putstring("\n\n");
        console_putstring(x);
        console_putstring("\n\n -- System halted\n");

        while(1);       /* Halt */
}

void memdump(unsigned char* addr, unsigned long size)
{
	int i = 0;
	int j;

	while ( i < size)
	{
		printf("%08lx  ", (unsigned long)addr + i);

		for (j = 0; (j < 8) && (i + j < size); j++)
			printf("%02x ", addr[i+j]);
		printf(" ");
		for (j = 8; (j < 16) && (i + j < size); j++)
			printf("%02x ", addr[i+j]);

		printf(" |");
		for (j = 0; (j < 16) && (i + j < size); j++)
		{
			if ( (addr[i+j] > 31) && (addr[i+j] < 128) )
				printf("%c", addr[i+j]);
			else
				printf(".");
		}
		printf("|\n");

		i += j;
	}
}
