/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 * Some parts from bootX, (c) BenH
 *
 */

#ifndef __ENTER_KERNELPPC_H__
#define __ENTER_KERNELPPC_H__

#include "misc.h"

typedef u_int32_t float_reg_t[2]; 

typedef struct PPCRegisterList
{
	u_int32_t	PC;
	u_int32_t	GPR[32];
	float_reg_t	FPR[32];
} PPCRegisterList;

extern void enter_kernelPPC(unsigned long addr, PPCRegisterList* regs);

#endif
