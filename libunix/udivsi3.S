/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 * Some parts from libgcc routines for 68000 w/o floating-point hardware.
 * Copyright (C) 1994, 1996, 1997, 1998 Free Software Foundation, Inc.
 *
 */

	.align 	2

	.globl	__udivsi3
__udivsi3:
	movel	%d2, %sp@-
	movel	%sp@(12), %d1	/* %d1 = divisor */
	movel	%sp@(8), %d0	/* %d0 = dividend */

	cmpl	#0x10000, %d1 /* divisor >= 2 ^ 16 ?   */
	jcc	__udivsi3L3		/* then try next algorithm */
	movel	%d0, %d2
	clrw	%d2
	swap	%d2
	divu	%d1, %d2          /* high quotient in lower word */
	movew	%d2, %d0		/* save high quotient */
	swap	%d0
	movew	%sp@(10), %d2	/* get low dividend + high rest */
	divu	%d1, %d2		/* low quotient */
	movew	%d2, %d0
	jra	__udivsi3L6

__udivsi3L3:	movel	%d1, %d2		/* use %d2 as divisor backup */
__udivsi3L4:	lsrl	#1, %d1	/* shift divisor */
	lsrl	#1, %d0	/* shift dividend */
	cmpl	#0x10000, %d1 /* still divisor >= 2 ^ 16 ?  */
	jcc	__udivsi3L4
	divu	%d1, %d0		/* now we have 16-bit divisor */
	andl	#0xffff, %d0 /* mask out divisor, ignore remainder */

/* Multiply the 16-bit tentative quotient with the 32-bit divisor.  Because of
   the operand ranges, this might give a 33-bit product.  If this product is
   greater than the dividend, the tentative quotient was too large. */
	movel	%d2, %d1
	mulu	%d0, %d1		/* low part, 32 bits */
	swap	%d2
	mulu	%d0, %d2		/* high part, at most 17 bits */
	swap	%d2		/* align high part with low part */
	tstw	%d2		/* high part 17 bits? */
	jne	__udivsi3L5		/* if 17 bits, quotient was too large */
	addl	%d2, %d1		/* add parts */
	jcs	__udivsi3L5		/* if sum is 33 bits, quotient was too large */
	cmpl	%sp@(8), %d1	/* compare the sum with the dividend */
	jls	__udivsi3L6		/* if sum > dividend, quotient was too large */
__udivsi3L5:	subql	#1, %d0		/* adjust quotient */

__udivsi3L6:	movel	%sp@+, %d2
	rts
