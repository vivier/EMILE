/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

char *strcpy (char *__restrict __dest,
                      __const char *__restrict __src)
{
	while ( (*__dest ++ = *__src++) );

	return __dest;
}
