static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "libemile.h"

#ifdef USE_16BIT_CHECKSUM
int emile_checksum(unsigned char *addr, unsigned short length)
{
	int j;
	unsigned short sum = 0;

	for (j = 0; j < length; j++)
	{
		sum += addr[j];
		sum = (sum << 1) | (sum >> 15);
	}
	if (sum == 0)
		sum = 0xFFFF;

	return sum;
}
#else
unsigned short emile_checksum(unsigned char *addr, unsigned int length)
{
	unsigned int sum = 0;
	unsigned int i;

	for (i = 0; i < length; i++)
	{
		sum += addr[i];
		sum <<= 1;
		sum |= (sum & 0x00010000) ? 1 : 0;
	}

	return sum;
}
#endif
