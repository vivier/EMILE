/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
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
#include "libconfig.h"

static char parity[] = { 'n', 'o', 'e' };
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

enum {
	STDOUT_VGA = 1,
	STDOUT_MODEM = 2,
	STDOUT_PRINTER = 4,
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
	int drive, second, size;
	int8_t *configuration;
	char property[256];

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

	configuration = emile_second_get_configuration(fd);
	if (configuration == NULL)
	{
		perror("Cannot read header");
		close(fd);
		return 4;
	}

	close(fd);

	if (config_get_property(configuration, "vga", property) == 0)
		printf("Output to display enabled (%s)\n", property);
	else
		printf("Output to display disabled\n");

	if (config_get_property(configuration, "modem", property) == 0)
		printf("Output to serial port 0 (modem) enabled (%s)\n", property);
	else
		printf("Output to serial port 0 (modem) disabled\n");

	if (config_get_property(configuration, "printer", property) == 0)
		printf("Output to serial port 1 (printer) enabled (%s)\n", property);
	else
		printf("Output to serial port 1 (printer) disabled\n");

	if (config_get_property(configuration, "gestaltID", property) == 0)
		printf("Force Gestalt ID to %ld\n", strtol(property, NULL, 0));
	else
		printf("Gestalt ID is not modified\n");

	free(configuration);

	return 0;
}

static int set_output(char* image,
		      unsigned int enable_mask, unsigned int disable_mask, 
		      unsigned int bitrate0, int datasize0,
		      int parity0, int stopbits0,
		      unsigned int bitrate1, int datasize1,
		      int parity1, int stopbits1, int gestaltid)
{
	int drive, second, size;
	int fd;
	int ret;
	int8_t *configuration;
	char property[32];
	int offset;

	fd = open(image, O_RDWR);

	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	/* can work on an image or directly on second level file */

	ret = emile_first_get_param(fd, &drive, &second, &size);
	if (ret == EEMILE_UNKNOWN_FIRST)
	{
		/* should be a second level file */
		offset = lseek(fd, 0, SEEK_SET);
		if (offset == -1)
		{
			perror("Cannot go to buffer offset");
			close(fd);
			return 3;
		}
	}
	else
		offset = lseek(fd, 0, SEEK_CUR);

	configuration = emile_second_get_configuration(fd);
	if (configuration == NULL)
	{
		perror("Cannot read header");
		close(fd);
		return 4;
	}

	if (disable_mask & STDOUT_VGA)
		config_remove_property(configuration, "vga");
	if (disable_mask & STDOUT_MODEM)
		config_remove_property(configuration, "modem");
	if (disable_mask & STDOUT_PRINTER)
		config_remove_property(configuration, "printer");

	if (enable_mask & STDOUT_VGA)
		config_set_property(configuration, "vga", "default");
	if (enable_mask & STDOUT_MODEM)
	{
		sprintf(property, "%d%c%d+%d", bitrate0, parity[parity0], datasize0, stopbits0);
		config_set_property(configuration, "modem", property);
	}
	if (enable_mask & STDOUT_PRINTER)
	{
		sprintf(property, "%d%c%d+%d", bitrate1, parity[parity1], datasize1, stopbits1);
		config_set_property(configuration, "printer", property);
	}

	if (gestaltid == 0)
		config_remove_property(configuration, "gestaltID");
	else if (gestaltid != -1)
	{
		sprintf(property, "0x%x", gestaltid);
		config_set_property(configuration, "gestaltID", property);
	}

	ret = lseek(fd, offset, SEEK_SET);
	if (ret == -1)
	{
		perror("Cannot go to buffer offset");
		close(fd);
		return 3;
	}

	ret = emile_second_set_configuration(fd, configuration);
	if (ret)
	{
		perror("Cannot write header");
		close(fd);
		return 4;
	}
	close(fd);
	free(configuration);

	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	char* image = NULL;
	int option_index;
	int c;
	unsigned int enable_mask = 0;
	unsigned int disable_mask = 0;
	unsigned int last = 0;
	int width = 0, height = 0 , depth = 0;
	unsigned int bitrate0 = 9600, bitrate1 = 9600;
	int datasize0 = 8, datasize1 = 8;
	int stopbits0 = 1, stopbits1 = 1;
	int parity0 = 0, parity1 = 0;
	int gestaltid = -1;

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
			disable_mask |= STDOUT_MODEM;
			last = 0;
			break;
		case ARG_NOPRINTER:
			disable_mask |= STDOUT_PRINTER;
			last = 0;
			break;
		case ARG_DISPLAY:
			enable_mask |= STDOUT_VGA;
			last = STDOUT_VGA;
			break;
		case ARG_MODEM:
			enable_mask |= STDOUT_MODEM;
			last = STDOUT_MODEM;
			break;
		case ARG_PRINTER:
			enable_mask |= STDOUT_PRINTER;
			last = STDOUT_PRINTER;
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
			if (last == STDOUT_MODEM)
				bitrate0 = atol(optarg);
			else if (last == STDOUT_PRINTER)
				bitrate1 = atol(optarg);
			else
			{
				fprintf(stderr,
				"missing --modem or --printer\n");
				return 1;
			}
			break;
		case ARG_DATASIZE:
			if (last == STDOUT_MODEM)
				datasize0 = atol(optarg);
			else if (last == STDOUT_PRINTER)
				datasize1 = atol(optarg);
			else
			{
				fprintf(stderr,
				"missing --modem or --printer\n");
				return 1;
			}
			break;
		case ARG_PARITY:
			if (last == STDOUT_MODEM)
				parity0 = atol(optarg);
			else if (last == STDOUT_PRINTER)
				parity1 = atol(optarg);
			else
			{
				fprintf(stderr,
				"missing --modem or --printer\n");
				return 1;
			}
			break;
		case ARG_STOPBITS:
			if (last == STDOUT_MODEM)
				stopbits0 = atol(optarg);
			else if (last == STDOUT_PRINTER)
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
	if ( (enable_mask == 0) && (disable_mask == 0) && (gestaltid == -1))
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
