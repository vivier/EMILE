/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "libemile.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s -f <first level> -s <second level> -i <kernel image> -r <ramdisk> -b <buffer size> <image>\n", argv[0]);
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

int main(int argc, char** argv)
{
	char* first_level = NULL;
	char* second_level = NULL;
	char* kernel_image = NULL;
	char* ramdisk = NULL;
	unsigned long buffer_size = 0;
	char* image = NULL;
	int i;
	int ret;

	if ((argc != 12) && (argc != 10) && (argc != 8) )
	{
		usage(argc, argv);
		return 1;
	}

	for (i = 1; i < argc; i++)
	{
		if (strcmp("-f", argv[i]) == 0)
		{
			if (first_level != NULL)
			{
				fprintf(stderr, "Duplicate definition of -f\n");
				return 1;
			}
			i++;
			if (i < argc)
				first_level = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -f\n");
				return 1;
			}
		}
		else if (strcmp("-s", argv[i]) == 0)
		{
			if (second_level != NULL)
			{
				fprintf(stderr, "Duplicate definition of -s\n");
				return 1;
			}
			i++;
			if (i < argc)
				second_level = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -s\n");
				return 1;
			}
		}
		else if (strcmp("-i", argv[i]) == 0)
		{
			if (kernel_image != NULL)
			{
				fprintf(stderr, "Duplicate definition of -i\n");
				return 1;
			}
			i++;
			if (i < argc)
				kernel_image = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -i\n");
				return 1;
			}
		}
		else if (strcmp("-b", argv[i]) == 0)
		{
			if (buffer_size != 0)
			{
				fprintf(stderr, "Duplicate definition of -b\n");
				return 1;
			}
			i++;
			if (i < argc)
				buffer_size = atol(argv[i]);
			else
			{
				fprintf(stderr, "Missing argument for -b\n");
				return 1;
			}
		}
		else if (strcmp("-r", argv[i]) == 0)
		{
			if (ramdisk != 0)
			{
				fprintf(stderr, "Duplicate definition of -r\n");
				return 1;
			}
			i++;
			if (i < argc)
				ramdisk = argv[i];
			else
			{
				fprintf(stderr, "Missing argument for -r\n");
				return 1;
			}
		}
		else
		{
			if (image != NULL)
			{
				fprintf(stderr, "Unknown parameter %s\n", 
						argv[i]);
				usage(argc, argv);
				return 1;
			}
			image = argv[i];
		}
	}

	ret = emile_floppy_create_image(first_level, second_level, 
					kernel_image, ramdisk, buffer_size, 
					image);
	return ret;
}
