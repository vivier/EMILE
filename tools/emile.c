/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#include "libemile.h"

int verbose = 0;

static char *first_path = PREFIX "/boot/emile/first_scsi";
static char *second_path = PREFIX "/boot/emile/second_scsi";
static char *kernel_path = PREFIX "/boot/vmlinuz";
static char *partition = NULL;
static int buffer_size = 0;

static struct option long_options[] =
{
	{"scanbus", 0, NULL, 0},
	{"backup", 0, NULL, 0},
	{"restore", 0, NULL, 0},
	{"verbose", 0, NULL, 'v'},
	{"first", 1, NULL, 'f'},
	{"second", 1, NULL, 's'},
	{"kernel", 1, NULL, 'k'},
	{"buffer", 1, NULL, 'b'},
	{"partition", 1, NULL, 'p'},
	{"help", 0, NULL, 'h'},
	{NULL,0,NULL,0}
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

/* first second kernel buffer_size command_line */

int main(int argc, char **argv)
{
	int c;
	int option_index = 0;
	int action_scanbus = 0;

	while(1)
	{
		c = getopt_long(argc, argv, "vhf:s:k:b:", long_options, &option_index);
		if (c == -1)
			break;
		switch(c)
		{
		case 0:
			if (option_index == 0)
				action_scanbus = 1;
			break;
		case 'v':
			verbose++;
			break;
		case 'h':
		case '?':
			usage(argc, argv);
			return 0;
		case 'f':
			first_path = optarg;
			break;
		case 's':
			second_path = optarg;
			break;
		case 'k':
			kernel_path = optarg;
			break;
		case 'b':
			buffer_size = atoi(optarg);
			break;
		}
	}

	if (action_scanbus) {
		scanbus();
		return 0;
	}

	printf("first:       %s\n", first_path);
	printf("second:      %s\n", second_path);
	printf("kernel:      %s\n", kernel_path);
	printf("buffer size: %d\n", buffer_size);
	printf("partition:   %s\n", partition);
#if 0
	int fd;
	int ret;

	/* set kernel info in second level */

	fd = open(argv[2], O_RDWR);	/* second */
	if (fd == -1)
	{
		perror("Cannot open second stage");
		return 1;
	}

	/* set buffer size */

	printf("Setting buffer size to %d\n", atoi(argv[4]));
	ret = emile_second_set_buffer_size(fd, atoi(argv[4]));

	/* set cmdline */

	lseek(fd, 0, SEEK_SET);
	printf("Setting command line to %s\n", argv[5]);
	ret = emile_second_set_cmdline(fd, argv[5]);

	/* set kernel info */

	lseek(fd, 0, SEEK_SET);
	ret = emile_second_set_kernel_scsi(fd, argv[3]);

	close(fd);

	/* set second info in first level */

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open first stage");
		return 1;
	}

	ret = emile_first_set_param_scsi(fd, argv[2]);
	close(fd);
#endif
	
	return 0;
}
