/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __ENTER_KERNEL_H__
#define __ENTER_KERNEL_H__

typedef void (*entry_t) (unsigned long , unsigned long , unsigned long, unsigned long );
typedef void (*disable_cache_t) (void);

#include "enter_kernelnoMMU.h"
#ifdef USE_MMU030
#include "enter_kernel030.h"
#include "MMU030.h"
#endif
#ifdef USE_MMU040
#include "enter_kernel040.h"
#include "MMU040.h"
#endif
#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#ifdef ARCH_PPC
#include "enter_kernelPPC.h"
#endif

extern void enter_kernel_init(void);
extern unsigned long enter_kernel;
extern unsigned long end_enter_kernel;
extern disable_cache_t disable_cache;

#endif
