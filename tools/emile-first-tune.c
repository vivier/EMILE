/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "emile-first.h"

#define TUNE_DRIVE	0x0001
#define TUNE_OFFSET	0x0002
#define TUNE_SIZE	0x0004

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [-d <drive>][-o <offset>][-s <size>] <image>\n", argv[0]);
	fprintf(stderr, "\n     set first level boot block info\n");
	fprintf(stderr, "     -d <drive>  : set the drive number (default 1)\n");
	fprintf(stderr,	"     -o <offset> : set offset of second level in bytes\n");
	fprintf(stderr,	"     -s <size>   : set size of second level in bytes\n");
	fprintf(stderr, "Display current values if no flags provided\n");
}

int first_tune( char* image, unsigned short tune_mask, int drive_num, 
		int file_ref, int second_offset, int second_size)
{
	int fd;
	eBootBlock_t firstBlock;
	int ret;

	fd = open(image, O_RDWR);
	if (fd == -1)
	{
		perror("Cannot open image file");
		return 2;
	}

	ret = read(fd, &firstBlock, sizeof(firstBlock));
	if (ret != sizeof(firstBlock))
	{
		perror("Cannot read first level boot block");
		close(fd);
		return 3;
	}

	if ( strncmp( firstBlock.boot_block_header.SysName+1,
		      "Mac Bootloader", 14) == 0 )
	{
		printf("EMILE boot block identified\n\n");

		if (tune_mask & TUNE_DRIVE)
		{
			printf("Set drive number to %d\n", drive_num);
			firstBlock.second_param_block.ioVRefNum = drive_num;
		}

		if (tune_mask & TUNE_OFFSET)
		{
			printf("Set second level offset to %d\n", second_offset);
			firstBlock.second_param_block.ioPosOffset = second_offset;
		}

		if (tune_mask & TUNE_SIZE)
		{
			printf("Set second level size to %d\n", second_size);
			firstBlock.second_param_block.ioReqCount = second_size;
		}

		if (tune_mask == 0)
		{
			printf("Drive number: %d\n", 
				firstBlock.second_param_block.ioVRefNum);
			printf("File reference number: %d\n", 
				firstBlock.second_param_block.ioRefNum);
			printf("Second level size: %ld\n", 
				firstBlock.second_param_block.ioReqCount);
			printf("Second level offset: %ld\n", 
				firstBlock.second_param_block.ioPosOffset);
		}
		else
		{
			ret = lseek(fd, 0, SEEK_SET);
			if (ret != 0)
			{
				perror("Cannot rewind !");
				close(fd);
				return 5;
			}
			ret = write(fd, &firstBlock, sizeof(firstBlock));
			if (ret != sizeof(firstBlock))
			{
				perror("Cannot write first level boot block");
				close(fd);
				return 6;
			}
		}
	}
	else
	{
		fprintf(stderr, "\nThis is not an EMILE boot block !\n");
	}

	close(fd);
	return 0;
}

int main(int argc, char** argv)
{
	int ret;
	int cargc;
	char** cargv;
	char* image;
	unsigned short tune_mask;
	int drive_num, file_ref, second_offset, second_size;

	ASSERT_BBH(
	{fprintf(stderr,"Internal Error: Bad BootBlkHdr size\n"); exit(1);});
	ASSERT_PBR(
	{fprintf(stderr,"Internal Error: Bad ParamBlockRec size\n"); exit(1);});
	ASSERT_BB(
	{fprintf(stderr,"Internal Error: Bad boot block size\n"); exit(1);});

	tune_mask = 0;
	image = NULL;
	cargc = argc - 1;
	cargv = argv + 1;
	while (cargc > 0)
	{
		if (strcmp(*cargv, "-d") == 0)
		{
			tune_mask |= TUNE_DRIVE;
			cargv++;
			cargc--;
			if (cargv == 0)
			{
				fprintf(stderr, "-d needs drive number\n");
				usage(argc, argv);
				return 1;
			}
			drive_num = atoi(*cargv);
			cargv++;
			cargc--;
		}
		else if (strcmp(*cargv, "-o") == 0)
		{
			tune_mask |= TUNE_OFFSET;
			cargv++;
			cargc--;
			if (cargv == 0)
			{
				fprintf(stderr, "-o needs offset\n");
				usage(argc, argv);
				return 1;
			}
			second_offset = atoi(*cargv);
			cargv++;
			cargc--;
		}
		else if (strcmp(*cargv, "-s") == 0)
		{
			tune_mask |= TUNE_SIZE;
			cargv++;
			cargc--;
			if (cargv == 0)
			{
				fprintf(stderr, "-s needs size\n");
				usage(argc, argv);
				return 1;
			}
			second_size = atoi(*cargv);
			cargv++;
			cargc--;
		}
		else
		{
			if (image != NULL)
			{
				fprintf(stderr, "Duplicate filename %s %s\n",
					image, *cargv);
				usage(argc, argv);
				return 1;
			}

			image = *cargv;
			cargv++;
			cargc--;
		}
	}

	if (image == NULL)
	{
		fprintf(stderr, "Missing filename to apply tuning\n");
		usage(argc, argv);
		return 1;
	}

	ret = first_tune( image, tune_mask, drive_num, file_ref, 
			  second_offset, second_size);

	return ret;
}
