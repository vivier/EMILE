/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "lowmem.h"
#include "bank.h"
#include "memory.h"
#include "uncompress.h"
#include "bootinfo.h"
#include "console.h"
#include "arch.h"
#include "misc.h"
#include "glue.h"
#include "load.h"

typedef void (*entry_t) (unsigned long , unsigned long , unsigned long );

extern void enter_kernel030(unsigned long addr, unsigned long size, unsigned long dest);
extern char end_enter_kernel030;
extern void enter_kernel040(unsigned long addr, unsigned long size, unsigned long dest);
extern char end_enter_kernel040;

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define BI_ALLOC_SIZE	(4096L)		// Allocate 4K for bootinfo
#define KERNEL_ALIGN	(256L * 1024L)	// Kernel alignment, on 256K boundary

struct first_level_info {
	unsigned long kernel_image_offset;
	unsigned long kernel_image_size;
	unsigned long kernel_size;
	unsigned long ramdisk_offset;
	unsigned long ramdisk_size;
	char command_line[CL_SIZE];
};

int start(struct first_level_info* info)
{
#ifdef	TARGET_M68K
	char * kernel;
	unsigned long physImage;
	entry_t entry;
	int ret;
	unsigned long start_mem;
	unsigned long aligned_size;
	unsigned long aligned_addr;
	unsigned long enter_kernel;
	unsigned long end_enter_kernel;
	unsigned long kernel_image_start;
	unsigned long ramdisk_start;
	int uncompressed_size;
#endif

	printf("Early Macintosh Image LoadEr\n");
	printf("EMILE v"VERSION" (c) 2004 Laurent Vivier\n");
	printf("This is free software, redistribute it under GPL\n");

	arch_init();

	init_memory_map();

#ifdef BANK_DUMP
	bank_dump();
#endif

	/* load kernel */

	printf("vmlinux %s\n", info->command_line);
	printf("Loading kernel...\n");
	kernel_image_start = (unsigned long)load_image(
				(unsigned long)info->kernel_image_offset, 
				info->kernel_image_size);
	printf("Kernel image loaded at 0x%lx\n", kernel_image_start);
	printf("Kernel image size is %ld Bytes\n", info->kernel_image_size);

#ifdef	TARGET_PPC

	if (arch_type == gestalt68k)
	{
		error("You're trying to boot a powerPC kernel on 680x0 Machine\n");
	}

	/* FIXME: add some stuff to start 3rd level (powerPC) */

	while(1);

	return 0;

#elif	defined(TARGET_M68K)

	if (arch_type == gestaltPowerPC)
	{
		error("You're trying to boot a m68k kernel on powerPC Machine\n");
	}

	printf("Available Memory: %ld kB\n", bank_mem_avail() / 1024);


	if (info->kernel_image_size != 0)
	{
		if (info->kernel_size == 0)
			info->kernel_size = info->kernel_image_size * 3;

		/* add KERNEL_ALIGN if we have to align
		 * and BI_ALLOC_SIZE for bootinfo
		 */

		printf("Allocating %ld bytes for kernel\n", info->kernel_size);
		kernel = (char*)malloc(info->kernel_size + 4 + BI_ALLOC_SIZE);
		if (kernel == 0)
		{
			printf("cannot allocate %ld bytes\n", info->kernel_size);
			while(1);
		}

		/* align kernel address to a 4 byte word */

		kernel = (unsigned char*)(((unsigned long)kernel + 3) & 0xFFFFFFFC);
		uncompressed_size = uncompress(kernel, (char*)kernel_image_start);
		printf("\n");
	}
	else
	{
		error("Kernel is missing !!!!\n");
	}

	/* free kernel image */

	free((void*)kernel_image_start);

	/* load ramdisk if needed */

	if (info->ramdisk_size != 0)
	{
		printf("Loading RAMDISK...\n");
		ramdisk_start = (unsigned long)load_image(
					(unsigned long)info->ramdisk_offset, 
					info->ramdisk_size);
		printf("RAMDISK loaded at 0x%lx\n", ramdisk_start);
		printf("RAMDISK size is %ld Bytes\n", info->ramdisk_size);
	}
	else
	{
		ramdisk_start = 0;
		printf("no RAMDISK\n");
	}

	ret = logical2physical((unsigned long)kernel, &physImage);

	/* initialize bootinfo structure */

	bootinfo_init(info->command_line, 
		      (char*)ramdisk_start, info->ramdisk_size);

	/* compute final address of kernel */

	aligned_size = boot_info.memory[0].addr & (KERNEL_ALIGN - 1);
	if ( aligned_size > 0 )
	{
		aligned_size = KERNEL_ALIGN - aligned_size;
		aligned_addr = boot_info.memory[0].addr + aligned_size;
		aligned_size = boot_info.memory[0].size - aligned_size;
		boot_info.memory[0].addr = aligned_addr;
		boot_info.memory[0].size = aligned_size;
	}

	/* set bootinfo at end of kernel image */

	set_kernel_bootinfo(kernel + uncompressed_size);

	/* disable interrupt */

	asm("ori.w #0x0700,%sr");

	/* disable and flush cache */

	asm("lea 0x0808, %%a1; movec %%a1, %%cacr"::: "%a1");
	
	/* where is mapped my boot function ? */
	
	if (mmu_type == gestalt68040MMU)
	{
		enter_kernel = (unsigned long)enter_kernel040;
		end_enter_kernel = (unsigned long)&end_enter_kernel040;
	}
	else
	{
		enter_kernel = (unsigned long)enter_kernel030;
		end_enter_kernel = (unsigned long)&end_enter_kernel030;
	}

	ret = logical2physical(enter_kernel, (unsigned long*)&entry);

	if ( (ret == 0) && (enter_kernel != (unsigned long)entry) )
	{
		unsigned long logi;
		unsigned long size = end_enter_kernel - enter_kernel;

		logi = console_get_video();
		ret = logical2physical(logi, (unsigned long*)&entry);
	

		memcpy((char*)logi, (char*)enter_kernel, size);
		memcpy((char*)entry, (char*)enter_kernel, size);

	}

	start_mem = boot_info.memory[0].addr + PAGE_SIZE;

	printf("\n");
	printf("Physical address of kernel will be 0x%08lx\n", start_mem);
	printf("Ok, booting the kernel.\n");
	entry(physImage, uncompressed_size + BI_ALLOC_SIZE, start_mem);

	return 0;

#endif	/* TARGET_M68K */
}
