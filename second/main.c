/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>

#include "lowmem.h"
#include "MMU.h"
#include "memory.h"
#include "uncompress.h"
#include "bootinfo.h"

extern char _kernel_start;
extern char _kernel_end;
extern char _KERNEL_SIZE;

extern void enter_kernel(char* addr, unsigned long size);

#define BI_ALLOC_SIZE	(4096L)		// Allocate 4K for bootinfo


int main(int argc, char** argv)
{
	char * kernel;
	char* kernel_image_start = &_kernel_start;
	unsigned long kernel_image_size = &_kernel_end - &_kernel_start;
	unsigned long kernel_size = (unsigned long)&_KERNEL_SIZE;
	unsigned long physEntry;
	int i;

	printf("Early Macintosh Image LoadEr\n");
	printf("EMILE v"VERSION" (c) 2004 Laurent Vivier\n");
	printf("This is free software, redistribute it under GPL\n");

	printf("Kernel image found at %p\n", kernel_image_start);
	printf("Kernel image size is %ld Bytes\n", kernel_image_size);

	printf("Available Memory: %ld KB\n", MemTop / 1024);
	printf("Physical memory map:\n");
	for (i = 0; i < memory_map.bank_number; i++)
	{
		printf("%d: 0x%08lx -> 0x%08lx mapped to 0x%08lx\n", i, 
			memory_map.bank[i].physAddr,
			memory_map.bank[i].physAddr + memory_map.bank[i].size,
			memory_map.bank[i].logiAddr);
	}

	if (kernel_image_size != 0)
	{
		/* add KERNEL_ALIGN if we have to align
		 * and BI_ALLOC_SIZE for bootinfo
		 */

		kernel = (char*)malloc(kernel_size + 4 + BI_ALLOC_SIZE);
		if (kernel == 0)
		{
			printf("ERROR: cannot allocate %ld bytes\n", kernel_size);
			while (1);
		}

		/* align kernel address to a 4 byte word */

		kernel = (unsigned char*)(((unsigned long)kernel + 3) & 0xFFFFFFFC);
		uncompress(kernel);
	}
	else
	{
		printf("Kernel is missing !!!!\n");
		while(1) ;
	}

	printf("\nOk, booting the kernel.\n");

	logical2physical((unsigned long)kernel, &physEntry);
	printf("Physical address of kernel is 0x%08lx\n", physEntry);

	set_kernel_bootinfo(kernel + kernel_size);
	
	enter_kernel((unsigned char*)physEntry, kernel_size + BI_ALLOC_SIZE);

	return 0;
}
