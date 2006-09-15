/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

int strlen(const char* s)
{
	int len;

	if (!s) return 0;

	len = 0;
	while (*s++) len++;

	return len;
}
