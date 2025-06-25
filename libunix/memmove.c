/*
 *
 * (c) 2025 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>

void *memmove(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	if (d == s || n == 0)
		return dest;

	if (d < s)
	{
		while (n--)
			*d++ = *s++;
	}
	else
	{
		d += n;
		s += n;
		while (n--)
			*--d = *--s;
	}

	return dest;
}
