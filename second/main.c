/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include "lowmem.h"
#include "bank.h"
#include "memory.h"
#include "uncompress.h"
#include "bootinfo.h"
#include "arch.h"
#include "misc.h"
#include "glue.h"
#include "load.h"
#include "console.h"
#include "vga.h"

typedef void (*entry_t) (unsigned long , unsigned long , unsigned long );
typedef void (*disable_cache_t) (void);

#ifdef ARCH_M68K
extern void enter_kernel030(unsigned long addr, unsigned long size, unsigned long dest);
extern char end_enter_kernel030;
extern void MMU030_disable_cache(void);
extern void enter_kernel040(unsigned long addr, unsigned long size, unsigned long dest);
extern char end_enter_kernel040;
extern void MMU040_disable_cache(void);
#endif
#ifdef ARCH_PPC
extern void enter_kernelPPC(unsigned long addr, unsigned long size, unsigned long dest);
extern char end_enter_kernelPPC;
extern void PPC_disable_cache(void);
#endif

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define BI_ALLOC_SIZE	(4096L)		// Allocate 4K for bootinfo
#define KERNEL_ALIGN	(256L * 1024L)	// Kernel alignment, on 256K boundary

int start(emile_l2_header_t* info)
{
	char * kernel;
	unsigned long physImage;
	entry_t entry;
	disable_cache_t disable_cache;
	int ret;
	unsigned long start_mem;
	unsigned long aligned_size;
	unsigned long aligned_addr;
	unsigned long enter_kernel;
	unsigned long end_enter_kernel;
	unsigned long kernel_image_start;
	unsigned long ramdisk_start;
	int uncompressed_size;

	printf("Early Macintosh Image LoadEr");
#if defined(ARCH_M68K)
	printf(" for Motorola 680x0\n");
#elif defined(ARCH_PPC)
	printf(" for PowerPC\n");
#else
	printf(" (unknown processor)\n");
#endif
	printf("EMILE v"VERSION" (c) 2004,2005 Laurent Vivier\n");
	printf("This is free software, redistribute it under GPL\n");

	if (!EMILE_COMPAT(EMILE_03_SIGNATURE, info->signature))
		error("Bad header signature !\n");

	arch_init();

	init_memory_map();

#ifdef BANK_DUMP
	bank_dump();
#endif

	printf("Available Memory: %ld kB\n", bank_mem_avail() / 1024);

	if (info->gestaltID != 0) {
		machine_id = info->gestaltID;
		printf("User forces gestalt ID to %ld\n", machine_id);
	}

	/* where is mapped my boot function ? */
	
#ifdef ARCH_M68K
	if (mmu_type == gestalt68040MMU)
	{
		enter_kernel = (unsigned long)enter_kernel040;
		end_enter_kernel = (unsigned long)&end_enter_kernel040;
		disable_cache = MMU040_disable_cache;
	}
	else
	{
		enter_kernel = (unsigned long)enter_kernel030;
		end_enter_kernel = (unsigned long)&end_enter_kernel030;
		disable_cache = MMU030_disable_cache;
	}
#endif
#ifdef ARCH_PPC
	enter_kernel = (unsigned long)enter_kernelPPC;
	end_enter_kernel = (unsigned long)&end_enter_kernelPPC;
	disable_cache = PPC_disable_cache;
#endif

	/* load kernel */

	printf("vmlinux %s\n", info->command_line);
#ifdef SCSI_SUPPORT
	info->kernel_image_offset = (unsigned long)info->kernel_image_offset + (unsigned long)info;
#endif
	printf("Kernel image size is %d Bytes\n", info->kernel_image_size);

	/* allocate memory for kernel */

	if (info->kernel_size == 0)	/* means uncompressed */
		kernel_image_start = (unsigned long)malloc_contiguous(
					info->kernel_image_size +
					BI_ALLOC_SIZE +
					end_enter_kernel - enter_kernel + 4);
	else
		kernel_image_start = (unsigned long)malloc(
						info->kernel_image_size + 4);

	kernel_image_start = (kernel_image_start + 3) & 0xFFFFFFFC;
	printf("Kernel image base at 0x%lx\n", kernel_image_start);
	if (kernel_image_start == 0)
		error("Cannot allocate memory\n");

	/* load kernel */

	printf("Loading kernel...\n");
	ret = load_image((unsigned long)info->kernel_image_offset, 
			 info->kernel_image_size, (char*)kernel_image_start);
	if (ret == -1)
		error("Cannot load kernel image\n");

	/* uncompress kernel if needed */

	if (info->kernel_image_size == 0)
		error("Kernel is missing !!!!\n");
	else if (info->kernel_size == 0)
	{
		/* kernel is not compressed, execute in place */

		kernel = (char*)kernel_image_start;
		uncompressed_size = info->kernel_image_size;
	}
	else
	{
		/* add KERNEL_ALIGN if we have to align
		 * and BI_ALLOC_SIZE for bootinfo
		 */

		printf("Allocating %d bytes for kernel\n", info->kernel_size);
		kernel = (char*)malloc_contiguous(info->kernel_size + 4 + BI_ALLOC_SIZE +
					end_enter_kernel - enter_kernel);
		if (kernel == 0)
		{
			printf("cannot allocate %d bytes\n", info->kernel_size);
			while(1);
		}

		/* align kernel address to a 4 byte word */

		kernel = (unsigned char*)(((unsigned long)kernel + 3) & 0xFFFFFFFC);
		uncompressed_size = uncompress(kernel, (char*)kernel_image_start);
		printf("\n");

		/* free kernel image */

		free((void*)kernel_image_start);
	}

	if (!check_full_in_bank((unsigned long)kernel, uncompressed_size))
		error("Kernel between two banks, contact maintainer\n");

	/* copy enter_kernel at end of kernel */

	memcpy((char*)kernel + uncompressed_size + BI_ALLOC_SIZE,
	       (char*)enter_kernel, end_enter_kernel - enter_kernel);

	end_enter_kernel = (unsigned long)kernel + uncompressed_size 
			   + BI_ALLOC_SIZE + (end_enter_kernel - enter_kernel);
	enter_kernel = (unsigned long)kernel + BI_ALLOC_SIZE 
		       + uncompressed_size;

	/* load ramdisk if needed */

	if (info->ramdisk_size != 0)
	{
		printf("RAMDISK size is %d Bytes\n", info->ramdisk_size);
		ramdisk_start = (unsigned long)malloc_contiguous(
							info->ramdisk_size + 4);
		ramdisk_start = (ramdisk_start + 3) & 0xFFFFFFFC;
		printf("RAMDISK base at 0x%lx\n", ramdisk_start);

		printf("Loading RAMDISK...\n");
		ret = load_image((unsigned long)info->ramdisk_offset, 
				 info->ramdisk_size, (char*)ramdisk_start);
		if (ret == -1)
			error("Cannot load ramdisk\n");

		if (!check_full_in_bank(ramdisk_start, info->ramdisk_size))
			error("ramdisk between two banks, contact maintainer\n");
	}
	else
	{
		ramdisk_start = 0;
		printf("no RAMDISK\n");
	}

	ret = logical2physical((unsigned long)kernel, &physImage);

	/* disable and flush cache */

	disable_cache();

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

	start_mem = boot_info.memory[0].addr + PAGE_SIZE;

	printf("\n");
	printf("Physical address of kernel will be 0x%08lx\n", start_mem);
	printf("Ok, booting the kernel.\n");

	ret = logical2physical(enter_kernel, (unsigned long*)&entry);

	if ( (ret == 0) && (enter_kernel != (unsigned long)entry) )
	{
		unsigned long logi;
		unsigned long size = end_enter_kernel - enter_kernel;

		logi = vga_get_video();
		ret = logical2physical(logi, (unsigned long*)&entry);
	

		memcpy((char*)logi, (char*)enter_kernel, size);
		memcpy((char*)entry, (char*)enter_kernel, size);
	}

	/* disable interrupt */

	asm("ori.w #0x0700,%sr");

	/* kick off */

	entry(physImage, uncompressed_size + BI_ALLOC_SIZE, start_mem);

	return 0;
}
