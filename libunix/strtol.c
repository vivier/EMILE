#include <stdlib.h>

static int my_isspace(int c)
{
	return (c == ' ') || (c == '\f') || (c == '\n') || 
		(c == '\r') || (c == '\t') || (c == '\v');
}

long int __strtol_internal (const char *nptr, char **endptr, int base, int group)
{
	long result = 0;
	int sign;

	while(my_isspace(*nptr))
		nptr++;

	if (*nptr == '-')
	{
		nptr++;
		sign = -1;
	} else if (*nptr == '+')
	{
		nptr++;
		sign = 1;
	} else
		sign = 1;

	if (base == 0)
	{
		if (*nptr == '0')
		{
			nptr++;
			if (*nptr == 'x')
			{
				base = 16;
				nptr++;
			} else
				base = 8;
		}
		else
			base = 10;
	}

	while (*nptr)
	{
		int digit;

		if ( (*nptr >= '0') && (*nptr <= '9') )
			digit = *nptr - '0';
		else if ( (*nptr >= 'a') && (*nptr <= 'z') )
			digit = *nptr - 'a' + 10;
		else if ( (*nptr >= 'A') && (*nptr <= 'Z') )
			digit = *nptr - 'A' + 10;
		else
			break;

		if (digit >= base)
			break;

		result = (result * base) + digit;

		nptr++;
	}
	if (endptr)
		*endptr = (char*)nptr;
	return sign * result;
}
