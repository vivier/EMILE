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
#include <getopt.h>

#include "libemile.h"

static char *parity[3] = { "None", "Odd", "Even" };

enum {
	ARG_NONE = 0,
	ARG_HELP = 'h',
	ARG_DISPLAY,
	ARG_WIDTH,
	ARG_HEIGHT,
	ARG_DEPTH,
	ARG_MODEM,
	ARG_PRINTER,
	ARG_BITRATE,
	ARG_DATASIZE,
	ARG_PARITY,
	ARG_STOPBITS,
	ARG_NODISPLAY,
	ARG_NOMODEM,
	ARG_NOPRINTER,
	ARG_GESTALTID,
};

static struct option long_options[] =
{
	{"help",	0, NULL,	ARG_HELP	},
	{"display",	0, NULL,	ARG_DISPLAY	},
	{"width",	1, NULL,	ARG_WIDTH	},
	{"height",	1, NULL,	ARG_HEIGHT	},
	{"depth",	1, NULL,	ARG_DEPTH	},
	{"modem",	0, NULL,	ARG_MODEM	},
	{"printer",	0, NULL,	ARG_PRINTER	},
	{"bitrate",	1, NULL,	ARG_BITRATE	},
	{"datasize",	1, NULL,	ARG_DATASIZE	},
	{"parity",	1, NULL,	ARG_PARITY	},
	{"stopbits",	1, NULL,	ARG_STOPBITS	},
	{"nodisplay",	0, NULL,	ARG_NODISPLAY	},
	{"nomodem",	0, NULL,	ARG_NOMODEM	},
	{"noprinter",	0, NULL,	ARG_NOPRINTER	},
	{"gestaltid",	1, NULL,	ARG_GESTALTID	},
	{NULL,		0, NULL,	0		},
};

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\n%s <file> --display [--width <width>] [--height <height>] [--depth <depth>]\n", argv[0]);
	fprintf(stderr, "     Enable output to display and set configuration\n");
	fprintf(stderr, "\n%s <image> --modem [--bitrate <bitrate>] [--datasize <datasize>] [--parity <parity>] [--stopbits <stopbits>]\n", argv[0]);
	fprintf(stderr, "     Enable output to serial port 0 (modem) and set configuration\n");
	fprintf(stderr, "\n%s <image> --printer [--bitrate <bitrate>] [--datasize <datasize>] [--parity <parity>] [--stopbits <stopbits>]\n", argv[0]);
	fprintf(stderr, "     Enable output to serial port 1 (printer) and set configuration\n");
	fprintf(stderr, "\n     <parity> is 0 for none, 1 for odd, 2 for even\n");
	fprintf(stderr, "\n%s <image> --nodisplay\n", argv[0]);
	fprintf(stderr, "     Disable output to display\n");
	fprintf(stderr, "\n%s <image> --nomodem\n", argv[0]);
	fprintf(stderr, "     Disable output to port 0 (modem)\n");
	fprintf(stderr, "\n%s <image> --noprinter\n", argv[0]);
	fprintf(stderr, "     Disable output to port 1 (printer)\n");
	fprintf(stderr, "\n%s <image> --gestaltid <id>\n", argv[0]);
	fprintf(stderr, "     Force <id> as gestalt id (set 0 to unset)\n");
	fprintf(stderr, "\n%s <image>\n", argv[0]);
	fprintf(stderr, "     Display current configuration\n");
	fprintf(stderr, "\nbuild: \n%s\n", SIGNATURE);
}

static int display_output(char* image)
{
	unsigned int console_mask;
	unsigned int bitrate0;
	int datasize0;
	int parity0;
	int stopbits0;
	unsigned int bitrate1;
	int datasize1;
	int parity1;
	int stopbits1;
	int gestaltid;
	int drive, second, size;

	int fd;
	int ret;

	fd = open(image, O_RDONLY);

	if (fd == -1)
	{
		perror("Cannot open file");
		return 2;
	}

	/* can work on an image or directly on second level file */

	ret = emile_first_get_param(fd, &drive, &second, &size);
	if (ret == EEMILE_UNKNOWN_FIRST)
	{
		/* should be a second level file */

		ret = lseek(fd, 0, SEEK_SET);
		if (ret == -1)
		{
			perror("Cannot go to buffer offset");
			close(fd);
			return 3;
		}
	}

	ret = emile_second_get_output(fd, &console_mask, &bitrate0,
					  &datasize0, &parity0, &stopbits0,
					  &bitrate1, &datasize1, &parity1,
					  &stopbits1, &gestaltid);
	if (ret)
	{
		perror("Cannot read header");
		close(fd);
		return 4;
	}

	close(fd);

	if (console_mask & STDOUT_VGA)
		printf("Output to display enabled\n");
	else
		printf("Output to display disabled\n");

	if (console_mask & STDOUT_SERIAL0)
		printf("Output to serial port 0 (modem) enabled\n");
	else
		printf("Output to serial port 0 (modem) disabled\n");

	printf("     Bitrate: %d Datasize: %d Parity: %s Stopbits: %d\n",
		bitrate0, datasize0, parity[parity0], stopbits0);

	if (console_mask & STDOUT_SERIAL1)
		printf("Output to serial port 1 (printer) enabled\n");
	else
		printf("Output to serial port 1 (printer) disabled\n");

	printf("     Bitrate: %d Datasize: %d Parity: %s Stopbits: %d\n",
		bitrate1, datasize1, parity[parity1], stopbits1);

	if (gestaltid)
		printf("Force Gestalt ID to %d\n", gestaltid);
	else
		printf("Gestalt ID is not modified\n");

	return 0;
}

static int set_output(char* image,
		      unsigned int enable_mask, unsigned int disable_mask, 
		      unsigned int bitrate0, int datasize0,
		      int parity0, int stopbits0,
		      unsigned int bitrate1, int datasize1,
		      int parity1, int stopbits1, int gestaltid)
{
	int fd;
	int ret;

	fd = open(image, O_RDWR);

	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = lseek(fd, FIRST_LEVEL_SIZE, SEEK_SET);
	if (ret == -1)
	{
		perror("Cannot go to buffer offset");
		close(fd);
		return 3;
	}

	ret = emile_second_set_output(fd, enable_mask, disable_mask,
				      bitrate0, datasize0, parity0, stopbits0,
				      bitrate1, datasize1, parity1, stopbits1,
				      gestaltid);
	if (ret)
	{
		perror("Cannot write header");
		close(fd);
		return 4;
	}
	close(fd);

	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	char* image;
	int option_index;
	int c;
	unsigned int enable_mask = 0;
	unsigned int disable_mask = 0;
	unsigned int last = 0;
	int width = 0, height = 0 , depth = 0;
	unsigned int bitrate0 = 0, bitrate1 = 0;
	int datasize0 = -1, datasize1 = -1;
	int stopbits0 = -1, stopbits1 = -1;
	int parity0 = -1, parity1 = -1;
	int gestaltid = 0;

	while(1)
	{
		c =  getopt_long(argc, argv, "h", long_options, 
				 &option_index);
		if (c == EOF)
			break;

		switch(c)
		{
		case ARG_HELP:
			usage(argc, argv);
			return 0;
		case ARG_GESTALTID:
			gestaltid = atol(optarg);
			break;
		case ARG_NODISPLAY:
			disable_mask |= STDOUT_VGA;
			last = 0;
			break;
		case ARG_NOMODEM:
			disable_mask |= STDOUT_SERIAL0;
			last = 0;
			break;
		case ARG_NOPRINTER:
			disable_mask |= STDOUT_SERIAL1;
			last = 0;
			break;
		case ARG_DISPLAY:
			enable_mask |= STDOUT_VGA;
			last = STDOUT_VGA;
			break;
		case ARG_MODEM:
			enable_mask |= STDOUT_SERIAL0;
			last = STDOUT_SERIAL0;
			break;
		case ARG_PRINTER:
			enable_mask |= STDOUT_SERIAL1;
			last = STDOUT_SERIAL1;
			break;
		case ARG_WIDTH:
			if (last != STDOUT_VGA)
			{
				fprintf(stderr, "missing --display\n");
				return 1;
			}
			width = atol(optarg);
			break;
		case ARG_HEIGHT:
			if (last != STDOUT_VGA)
			{
				fprintf(stderr, "missing --display\n");
				return 1;
			}
			height = atol(optarg);
			break;
		case ARG_DEPTH:
			if (last != STDOUT_VGA)
			{
				fprintf(stderr, "missing --display\n");
				return 1;
			}
			depth = atol(optarg);
			break;
		case ARG_BITRATE:
			if (last == STDOUT_SERIAL0)
				bitrate0 = atol(optarg);
			else if (last == STDOUT_SERIAL1)
				bitrate1 = atol(optarg);
			else
			{
				fprintf(stderr,
				"missing --modem or --printer\n");
				return 1;
			}
			break;
		case ARG_DATASIZE:
			if (last == STDOUT_SERIAL0)
				datasize0 = atol(optarg);
			else if (last == STDOUT_SERIAL1)
				datasize1 = atol(optarg);
			else
			{
				fprintf(stderr,
				"missing --modem or --printer\n");
				return 1;
			}
			break;
		case ARG_PARITY:
			if (last == STDOUT_SERIAL0)
				parity0 = atol(optarg);
			else if (last == STDOUT_SERIAL1)
				parity1 = atol(optarg);
			else
			{
				fprintf(stderr,
				"missing --modem or --printer\n");
				return 1;
			}
			break;
		case ARG_STOPBITS:
			if (last == STDOUT_SERIAL0)
				stopbits0 = atol(optarg);
			else if (last == STDOUT_SERIAL1)
				stopbits1 = atol(optarg);
			else
			{
				fprintf(stderr,
				"missing --modem or --printer\n");
				return 1;
			}
		}
	}
	if (optind < argc)
		image = argv[optind];
	if (image == NULL)
	{
		fprintf(stderr, "ERROR: missing image file name\n");
		usage(argc, argv);
		return 1;
	}

	if ( width || height || depth)
	{
		fprintf(stderr,
	"WARNING: setting display properties is not yet implemented !\n");
	}

	if (enable_mask & disable_mask) {
		fprintf(stderr, "Cannot enable and disable at same time\n");
		return 2;
	}
	if ( (enable_mask == 0) && (disable_mask == 0))
	{
		display_output(image);
		return 0;
	}
	ret = set_output(image, enable_mask, disable_mask, 
			 bitrate0, datasize0, parity0, stopbits0,
			 bitrate1, datasize1, parity1, stopbits1,
			 gestaltid);

	return ret;
}
