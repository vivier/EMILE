/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "lowmem.h"
#include "MMU.h"
#include "bank.h"
#include "memory.h"
#include "uncompress.h"
#include "bootinfo.h"
#include "console.h"

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
	typedef void (*entry_t) (unsigned char *physEntry, unsigned long size);
	entry_t entry;
	int ret;

	printf("Early Macintosh Image LoadEr\n");
	printf("EMILE v"VERSION" (c) 2004 Laurent Vivier\n");
	printf("This is free software, redistribute it under GPL\n");

	bank_dump();

	printf("Kernel image found at %p\n", kernel_image_start);
	printf("Kernel image size is %ld Bytes\n", kernel_image_size);

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

	ret = logical2physical((unsigned long)kernel, &physEntry);

	set_kernel_bootinfo(kernel + kernel_size);

	/* disable interrupt */

	asm("ori.w #0x0700,%sr");

	/* disable and flush cache */

	asm("lea 0x0808, %%a1; movec %%a1, %%cacr"::: "%a1");
	
	/* where is mapped my boot function ? */
	
	ret = logical2physical( (unsigned long)enter_kernel, 
				(unsigned long*)&entry);

	if ( (ret == 0) && 
	     ((unsigned long)enter_kernel != (unsigned long)entry) )
	{
		extern char end_enter_kernel;
		unsigned long logi;
		unsigned long size = (unsigned long)&end_enter_kernel - 
					(unsigned long)&enter_kernel;

		logi = console_get_video();
		ret = logical2physical(logi, (unsigned long*)&entry);
	

		memcpy((char*)logi, &enter_kernel, size);
		memcpy((char*)entry, &enter_kernel, size);

	}

	printf("\nOk, booting the kernel.\n");

	entry((unsigned char*)physEntry, kernel_size + BI_ALLOC_SIZE);

	return 0;
}
