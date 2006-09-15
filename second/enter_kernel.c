/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>

#include <macos/types.h>
#include <macos/gestalt.h>

#include "misc.h"
#include "arch.h"
#include "enter_kernel.h"

unsigned long enter_kernel;
unsigned long end_enter_kernel;
disable_cache_t disable_cache;

void enter_kernel_init(void)
{
#ifdef ARCH_M68K
	if (arch_type == gestalt68k)
	{
		if (mmu_type == gestalt68040MMU)
		{
#ifdef USE_MMU040
			enter_kernel = (unsigned long)enter_kernel040;
			end_enter_kernel = (unsigned long)&end_enter_kernel040;
			disable_cache = MMU040_disable_cache;
#else
			error("68040 MMU is not supported");
#endif
		}
		else if (mmu_type == gestalt68030MMU)
		{
#ifdef USE_MMU030
			enter_kernel = (unsigned long)enter_kernel030;
			end_enter_kernel = (unsigned long)&end_enter_kernel030;
			disable_cache = MMU030_disable_cache;
#else
			error("68030 MMU is not supported");
#endif
		}
		else if (mmu_type == gestalt68851)
		{
			error("MMU 68851 is not supported");
		}
		else if (mmu_type == gestaltNoMMU)
		{
			enter_kernel = (unsigned long)enter_kernelnoMMU;
			end_enter_kernel = (unsigned long)&end_enter_kernelnoMMU;
			disable_cache = noMMU_disable_cache;
		}
		else
			error("Unsupported MMU");
	}
#endif
#ifdef ARCH_PPC
	if (arch_type == gestaltPowerPC)
	{
		enter_kernel = NULL;
		end_enter_kernel = NULL;
		disable_cache = NULL;
		bootstrap_size = 0;
	}
	else
		error("EMILE doesn't support your architecture");
#endif
}
