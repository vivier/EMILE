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

static char *parity[3] = { "None", "Odd", "Even" };

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\n%s <image> --display [--width <width>] [--height <height>] [--depth <depth>]\n", argv[0]);
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

	int fd;
	int ret;

	fd = open(image, O_RDONLY);

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
	if (argc < 2)
	{
		usage(argc, argv);
		return 1;
	}

	image = argv[1];
	if (argc == 2)
		display_output(image);
	else
	{
		unsigned int enable_mask = 0;
		unsigned int disable_mask = 0;
		unsigned int last = 0;
		int i = 2;
		int width = 0, height = 0 , depth = 0;
		unsigned int bitrate0 = 0, bitrate1 = 0;
		int datasize0 = -1, datasize1 = -1;
		int stopbits0 = -1, stopbits1 = -1;
		int parity0 = -1, parity1 = -1;
		int gestaltid = 0;

		while (i < argc) {
			if (!strcmp("--help", argv[i])) {
				usage(argc, argv);
				return 0;
			} else if (!strcmp("--gestaltid", argv[i])) {
				i++;
				if (i >= argc) {
					usage(argc, argv);
					return 0;
				}
				gestaltid = atol(argv[i++]);
			} else if (!strcmp("--nodisplay", argv[i])) {
				disable_mask |= STDOUT_VGA;
				last = 0;
				i++;
			} else if (!strcmp("--nomodem", argv[i])) {
				disable_mask |= STDOUT_SERIAL0;
				last = 0;
			} else if (!strcmp("--noprinter", argv[i])) {
				disable_mask |= STDOUT_SERIAL1;
				last = 0;
				i++;
			} else if (!strcmp("--display", argv[i])) {
				enable_mask |= STDOUT_VGA;
				last = STDOUT_VGA;
				i++;
			} else if (!strcmp("--modem", argv[i])) {
				enable_mask |= STDOUT_SERIAL0;
				last = STDOUT_SERIAL0;
				i++;
			} else if (!strcmp("--printer", argv[i])) {
				enable_mask |= STDOUT_SERIAL1;
				last = STDOUT_SERIAL1;
				i++;
			} else if (last == STDOUT_VGA) {
				if (!strcmp("--width", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --width\n");
						return -1;
					}
					width = atol(argv[i++]);
				} else if (!strcmp("--height", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --height\n");
						return -1;
					}
					height = atol(argv[i++]);
				} else if (!strcmp("--depth", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --depth\n");
						return -1;
					}
					depth = atol(argv[i++]);
				}
				else {
					fprintf(stderr, "Unknown parameter %s\n", argv[i]);
					return -1;
				}
			} else if (last == STDOUT_SERIAL0) {
				if (!strcmp("--bitrate", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --bitrate\n");
						return -1;
					}
					bitrate0 = atol(argv[i++]);
				} else if (!strcmp("--datasize", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --datasize\n");
						return -1;
					}
					datasize0 = atol(argv[i++]);
				} else if (!strcmp("--parity", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --parity\n");
						return -1;
					}
					parity0 = atol(argv[i++]);
				} else if (!strcmp("--stopbits", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --stopbits\n");
						return -1;
					}
					stopbits0 = atol(argv[i++]);
				}
				else {
					fprintf(stderr, "Unknown parameter %s\n", argv[i]);
					return -1;
				}
			} else if (last == STDOUT_SERIAL1) {
				if (!strcmp("--bitrate", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --bitrate\n");
						return -1;
					}
					bitrate1 = atol(argv[i++]);
				} else if (!strcmp("--datasize", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --datasize\n");
						return -1;
					}
					datasize1 = atol(argv[i++]);
				} else if (!strcmp("--parity", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --parity\n");
						return -1;
					}
					parity1 = atol(argv[i++]);
				} else if (!strcmp("--stopbits", argv[i])) {
					i++;
					if (i >= argc)
					{
						fprintf(stderr, "Missing parameter for --stopbits\n");
						return -1;
					}
					stopbits1 = atol(argv[i++]);
				}
				else {
					fprintf(stderr, "Unknown parameter %s\n", argv[i]);
					return -1;
				}
			}
			else {
				fprintf(stderr, "Unknown parameter %s\n", argv[i]);
				return -1;
			}
		}

		if ( width || height || depth) {
			fprintf(stderr, "WARNING: setting display properties is not yet implemented !\n");
		}

		if (enable_mask & disable_mask) {
			fprintf(stderr, "Cannot enable and disable at same time\n");
			return -1;
		}
		ret = set_output(image, enable_mask, disable_mask, 
				 bitrate0, datasize0, parity0, stopbits0,
				 bitrate1, datasize1, parity1, stopbits1,
				 gestaltid);
	}

	return ret;
}
