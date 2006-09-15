/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

char *strcpy (char *__restrict __dest,
                      __const char *__restrict __src)
{
	while ( (*__dest ++ = *__src++) );

	return __dest;
}
