/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#define __NO_INLINE__

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

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
#include "switch_to_PPC.h"
extern u_int32_t _bootstrap_start;
extern u_int32_t _bootstrap_end;
#endif
#include "arch.h"
#include "misc.h"
#include "load.h"
#include "console.h"
#include "vga.h"
#include "driver.h"
#include "config.h"
#include "serial.h"

#include "enter_kernel.h"

#ifdef ARCH_M68K
#define KERNEL_ALIGN	(256L * 1024L)	// Kernel alignment, on 256K boundary
#if defined(__LINUX__)
#define BI_ALLOC_SIZE	(4096L)		// Allocate 4K for bootinfo
#elif defined(__NETBSD__)
#define BI_ALLOC_SIZE	(4096L)
#endif
#endif

typedef void (*loader_t)(void);

int start(emile_l2_header_t* info)
{
	char *kernel;
	loader_t loader;
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
	char *ramdisk_start;
	unsigned long kernel_size;
	unsigned long ramdisk_size;
	emile_config_t econfig;

	serial_init();
	console_init();
	arch_init();
	init_memory_map();
#ifdef BANK_DUMP
	bank_dump();
#endif
	enter_kernel_init();

	memset(&econfig, 0, sizeof(econfig));

retry:
	if (econfig.kernel != NULL)
		free(econfig.kernel);
	if (econfig.command_line != NULL)
		free(econfig.command_line);
	if (econfig.initrd != NULL)
		free(econfig.initrd);
	if (econfig.chainloader != NULL)
		free(econfig.chainloader);

	if (read_config(info, &econfig) != 0)
		error("cannot read configuration\n");
	if (econfig.chainloader)
	{
		loader = (loader_t)load_chainloader(econfig.chainloader);
		if (loader == NULL)
			goto retry;
		loader();
	}

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

	/* load kernel */

	kernel = load_kernel(econfig.kernel,
			     bootstrap_size, 
			     &start_mem, &entry_point, &kernel_size);
	if (kernel == NULL)
	{
		printf("Cannot load and uncompress kernel image\n");
		printf("Press any key");
		console_keypressed(0);
		goto retry;
	}

	/* load ramdisk if needed */

	if (econfig.initrd)
	{
		ramdisk_start = load_ramdisk(econfig.initrd, &ramdisk_size);
		if (ramdisk_start == NULL)
		{
			if (kernel != NULL)
				free(kernel);
			printf("Cannot open ramdisk\n");
			printf("Press any key");
			console_keypressed(0);
			goto retry;
		}
	}
	else
		ramdisk_start = 0;

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

		/* compute final address of kernel */

		if  (mmu_type == gestaltNoMMU)
		{
			unsigned long size = end_enter_kernel - enter_kernel;

#if defined(__LINUX__)
			/* initialize bootinfo structure */

			bootinfo_init(econfig.command_line, 
		      		ramdisk_start, ramdisk_size);

			/* set bootinfo at end of kernel image */

			set_kernel_bootinfo(kernel + kernel_size);

			physImage = (unsigned long)kernel;
#endif
			entry = (entry_t)(start_mem - size);

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

			bootinfo_init(econfig.command_line, 
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
		PPCRegisterList regs;

		bootx_init(econfig.command_line, ramdisk_start, ramdisk_size);

		regs.PC	     = (u_int32_t)&_bootstrap_start;

#define BOOT_KERNEL_STACK_SIZE 65536
		regs.GPR[1]  = (u_int32_t)malloc_contiguous(BOOT_KERNEL_STACK_SIZE) + BOOT_KERNEL_STACK_SIZE - 512;
		regs.GPR[2]  = 1;
		regs.GPR[3]  = 'BooX';
		regs.GPR[4]  = (u_int32_t)&bootx_infos;
		regs.GPR[5]  = 0;
		regs.GPR[6] = (int)&_bootstrap_start;
		regs.GPR[7]  = 0; // where bootstrap must be copied;
		regs.GPR[8]  = ((((&_bootstrap_end - &_bootstrap_start) + 4095) << 12) >> 12);
		regs.GPR[9]  = 0; //boot_map_addr;
		regs.GPR[10]  = (u_int32_t)kernel;
		regs.GPR[11]  = 0;

		switch_to_PPC(&regs);
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

	error("Kernel startup failed");

	return 0;
}
