/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * Some parts from bootX, (c) BenH
 *
 */

#ifndef __ENTER_KERNELPPC_H__
#define __ENTER_KERNELPPC_H__

#include "misc.h"

typedef unsigned long float_reg_t[2] __attribute__((packed));

typedef struct PPCRegisterList
{
	u_int32_t	PC;
	u_int32_t	GPR[32];
	float_reg_t	FPR[32];
} PPCRegisterList __attribute__((packed));

extern void enter_kernelPPC(unsigned long addr, PPCRegisterList* regs);

#endif
