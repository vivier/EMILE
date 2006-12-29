/*
 *
 * (c) 2004-2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#define __NO_INLINE__

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <macos/types.h>
#include <macos/gestalt.h>

#include "bank.h"
#include "memory.h"
#if defined(ARCH_M68K)
#if defined(__LINUX__)
#include "bootinfo.h"
#elif defined(__NETBSD__)
#include "bootenv.h"
#endif
#endif
#ifdef ARCH_PPC
#include "bootx.h"
#endif
#include "arch.h"
#include "misc.h"
#include "load.h"
#include "console.h"
#include "vga.h"
#include "driver.h"
#include "config.h"

#include "enter_kernel.h"

#ifdef ARCH_M68K
#define KERNEL_ALIGN	(256L * 1024L)	// Kernel alignment, on 256K boundary
#if defined(__LINUX__)
#define BI_ALLOC_SIZE	(4096L)		// Allocate 4K for bootinfo
#elif defined(__NETBSD__)
#define BI_ALLOC_SIZE	(4096L)
#endif
#endif


int start(emile_l2_header_t* info)
{
	char * kernel;
#ifdef ARCH_M68K
	entry_t entry;
	unsigned long physImage;
#ifdef USE_MMU
	unsigned long physical;
#ifdef __LINUX__
	unsigned long aligned_size;
	unsigned long aligned_addr;
#endif /* __LINUX__ */
#endif /* USE_MMU */
	int ret;
#endif /* ARCH_M68K */
	unsigned long start_mem;
	unsigned long entry_point;
	int bootstrap_size;
#ifdef ARCH_PPC
	PPCRegisterList regs;
#endif
	char *ramdisk_start;
	unsigned long kernel_size;
	unsigned long ramdisk_size;
	char *kernel_path;
	char *ramdisk_path;
	char *command_line;

	printf("EMILE v"VERSION" (c) 2004-2006 Laurent Vivier\n");

	arch_init();

	init_memory_map();

#ifdef BANK_DUMP
	bank_dump();
#endif

	printf("Available Memory: %ld kB\n", bank_mem_avail() / 1024);

	enter_kernel_init();

#ifdef ARCH_M68K
	if (arch_type == gestalt68k)
	{
#if defined(__LINUX__)
		/* and BI_ALLOC_SIZE for bootinfo */

		bootstrap_size = BI_ALLOC_SIZE + 
				 end_enter_kernel - enter_kernel;
#elif defined(__NETBSD__)
		bootstrap_size = BI_ALLOC_SIZE + 
				 end_enter_kernel - enter_kernel;
#endif
	}
	else
		error("EMILE doesn't support your architecture");
#endif
#ifdef ARCH_PPC
	if (arch_type == gestaltPowerPC)
	{
		bootstrap_size = end_enter_kernel - enter_kernel;
	}
	else
		error("EMILE doesn't support your architecture");
#endif

	if (read_config(info, &kernel_path, &command_line, &ramdisk_path) != 0)
		error("cannot read configuration\n");

	/* load kernel */

	kernel = load_kernel(kernel_path,
			     bootstrap_size, 
			     &start_mem, &entry_point, &kernel_size);
	if (kernel == NULL)
		error("Cannot load and uncompress kernel image\n");

#ifdef ARCH_M68K
	if (arch_type == gestalt68k)
	{
		unsigned long enter_size = end_enter_kernel - enter_kernel;

		/* copy enter_kernel at end of kernel */

		memcpy((char*)kernel +
			kernel_size + bootstrap_size - enter_size,
	       		(char*)enter_kernel, enter_size);

		end_enter_kernel = (unsigned long)kernel + 
				   kernel_size + bootstrap_size;
		enter_kernel = (unsigned long)kernel + 
			       bootstrap_size - enter_size + kernel_size;
	}
#endif

	/* load ramdisk if needed */

	if (ramdisk_path)
	{
		ramdisk_start = load_ramdisk(ramdisk_path, &ramdisk_size);
		if (ramdisk_start == NULL)
			error("Cannot open ramdisk\n");
	}
	else
	{
		ramdisk_start = 0;
		printf("no RAMDISK\n");
	}

#ifdef ARCH_M68K
	if (arch_type == gestalt68k)
	{
		/* compute final address of kernel */

		if  (mmu_type == gestaltNoMMU)
		{
			unsigned long size = end_enter_kernel - enter_kernel;

#if defined(__LINUX__)
			/* initialize bootinfo structure */

			bootinfo_init(command_line, 
		      		ramdisk_start, ramdisk_size);

			/* set bootinfo at end of kernel image */

			set_kernel_bootinfo(kernel + kernel_size);

			physImage = (unsigned long)kernel;
#endif
			entry = (entry_t)(start_mem - size);

			printf("\n");
			printf("Ok, booting the kernel.\n");

			memcpy(entry, (char*)enter_kernel, size);
		} else
#ifndef USE_MMU
			error("Unsupported MMU");
#else
		{
			ret = logical2physical((unsigned long)kernel, &physImage);

			/* disable and flush cache */

			disable_cache();

#if defined(__LINUX__)
			/* initialize bootinfo structure */

			bootinfo_init(command_line, 
		      		ramdisk_start, ramdisk_size);

			/* add KERNEL_ALIGN if we have to align */
		 
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

			set_kernel_bootinfo(kernel + kernel_size);

#elif defined(__NETBSD__)
			bootenv_init(kernel + kernel_size);
#endif

			printf("\n");
			printf("Ok, booting the kernel.\n");

			ret = logical2physical(enter_kernel, &physical);
			entry = (entry_t)physical;

			if ( (ret == 0) && (enter_kernel != (unsigned long)entry) )
			{
				unsigned long logi;
				unsigned long size = end_enter_kernel - enter_kernel;

				logi = vga_get_video();
				ret = logical2physical(logi, &physical);
				entry = (entry_t)physical;
	
				memcpy((char*)logi, (char*)enter_kernel, size);
				memcpy((char*)entry, (char*)enter_kernel, size);
			}
		}
#endif /* USE_MMU */
	}
	else
#ifndef ARCH_PPC
		error("EMILE doesn't support your architecture");
#endif
#endif /* ARCH_M68K */
#ifdef ARCH_PPC
	if (arch_type == gestaltPowerPC)
	{
		bootx_init(command_line, ramdisk_start, ramdisk_size);

		regs.PC      = (u_int32_t)kernel;
#define BOOT_KERNEL_STACK_SIZE 65536
		regs.GPR[1]  = (u_int32_t)malloc_contiguous(BOOT_KERNEL_STACK_SIZE)
				+ BOOT_KERNEL_STACK_SIZE - 512;
		regs.GPR[2]  = 0;
		regs.GPR[3]  = 'BooX';
		regs.GPR[4]  = (u_int32_t)&bootx_infos;

		/* Set up the info for BAT mapping on Nubus */

		regs.GPR[5]  = vga_get_videobase() & 0xFF800000UL;
		regs.GPR[11]  = 1;

		printf("\n");
		printf("Ok, booting the kernel.\n");
	}
	else
		error("EMILE doesn't support your architecture");
#endif

	turn_off_interrupts();

	 asm("ori.w #0x0700,%sr");

	/* kick off */

#ifdef ARCH_M68K
	if (arch_type == gestalt68k)
		entry(physImage, kernel_size + BI_ALLOC_SIZE, start_mem, entry_point);
#endif
#ifdef ARCH_PPC
	if (arch_type == gestaltPowerPC)
		enter_kernelPPC((unsigned long)kernel, &regs);
#endif

	error("Kernel startup failed");

	return 0;
}
