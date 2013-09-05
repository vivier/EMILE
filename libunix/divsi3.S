/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 * Some parts from libgcc routines for 68000 w/o floating-point hardware.
 * Copyright (C) 1994, 1996, 1997, 1998 Free Software Foundation, Inc.
 *
 */

	.align 	2

	.globl	__divsi3
__divsi3:
	movel	%d2, %sp@-

	moveq	#1, %d2	/* sign of result stored in %d2 (=1 or =-1) */
	movel	%sp@(12), %d1	/* %d1 = divisor */
	jpl	__divsi3L1
	negl	%d1
	negb	%d2		/* change sign because divisor <0  */
__divsi3L1:	movel	%sp@(8), %d0	/* %d0 = dividend */
	jpl	__divsi3L2
	negl	%d0
	negb	%d2

__divsi3L2:	movel	%d1, %sp@-
	movel	%d0, %sp@-
	bsr	__udivsi3	/* divide abs(dividend) by abs(divisor) */
	addql	#8, %sp

	tstb	%d2
	jpl	__divsi3L3
	negl	%d0

__divsi3L3:	movel	%sp@+, %d2
	rts
