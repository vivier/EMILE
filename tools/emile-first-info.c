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
#include <string.h>

#include "emile-first.h"

static void usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s <image>\n", argv[0]);
	fprintf(stderr, "\n     display first level boot block info\n");
}

static void pprint(char *string)
{
	int l = *string++;

	while (l-- != 0)
		putchar(*string++);
}

int first_info(char* image)
{
	int fd;
	eBootBlock_t firstBlock;
	int ret;

	fd = open(image, O_RDONLY);
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

	printf("Boot Block Header:\n\n");
	printf("Boot blocks signature: 0x%x\n", 
					firstBlock.boot_block_header.ID);
	printf("Entry point to bootcode: 0x%lX\n", 
					firstBlock.boot_block_header.Entry);
	printf("Boot blocks version number: %x\n", 
					firstBlock.boot_block_header.Version);
	printf("System filename: ");
	pprint(firstBlock.boot_block_header.SysName);
	putchar('\n');
	printf("Finder filename: ");
	pprint(firstBlock.boot_block_header.ShellName);
	putchar('\n');
	printf("Debugger1 filename: ");
	pprint(firstBlock.boot_block_header.Dbg1Name);
	putchar('\n');
	printf("Debugger2 filename: ");
	pprint(firstBlock.boot_block_header.Dbg2Name);
	putchar('\n');
	printf("Name of startup screen: ");
	pprint(firstBlock.boot_block_header.ScreenName);
	putchar('\n');
	printf("Name of startup program: ");
	pprint(firstBlock.boot_block_header.HelloName);
	putchar('\n');
	printf("Name of system scrap file: ");
	pprint(firstBlock.boot_block_header.ScrapName);
	putchar('\n');
	printf("Number of FCBs to allocate: %d\n", 
					firstBlock.boot_block_header.CntFCBs);
	printf("Number of event queue elements: %d\n",
					firstBlock.boot_block_header.CntEvts);
	printf("System heap size on 128K Mac: 0x%lx\n",
					firstBlock.boot_block_header.Heap128K);
	printf("System heap size on 256K Mac: 0x%lx\n",
					firstBlock.boot_block_header.Heap256K);
	printf("System heap size on all machines: 0x%lx\n",
					firstBlock.boot_block_header.SysHeapSize);

	if ( strncmp( firstBlock.boot_block_header.SysName+1,
		      "Mac Bootloader", 14) == 0 )
	{
		printf("\nEMILE boot block identified\n\n");

		printf("Drive number: %d\n", 
				firstBlock.second_param_block.ioVRefNum);
		printf("File reference number: %d\n", 
				firstBlock.second_param_block.ioRefNum);
		printf("Second level size: %ld\n", 
				firstBlock.second_param_block.ioReqCount);
		printf("Second level offset: %ld\n", 
				firstBlock.second_param_block.ioPosOffset);
	}

	close(fd);
	return 0;
}

int main(int argc, char** argv)
{
	int ret;

	ASSERT_BBH(
	{fprintf(stderr,"Internal Error: Bad BootBlkHdr size\n"); exit(1);});
	ASSERT_PBR(
	{fprintf(stderr,"Internal Error: Bad ParamBlockRec size\n"); exit(1);});
	ASSERT_BB(
	{fprintf(stderr,"Internal Error: Bad boot block size\n"); exit(1);});

	if (argc != 2)
	{
		usage(argc, argv);
		return 1;
	}

	ret = first_info(argv[1]);

	return ret;
}
