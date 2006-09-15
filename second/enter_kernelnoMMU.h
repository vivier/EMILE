/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __ENTER_KERNELNOMMU_H__
#define __ENTER_KERNELNOMMU_H__

extern void noMMU_disable_cache(void);

extern void enter_kernelnoMMU(unsigned long addr, unsigned long size, unsigned long dest, unsigned long entry);
extern char end_enter_kernelnoMMU;

#endif
