/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * Some parts from libgcc routines for 68000 w/o floating-point hardware.
 * Copyright (C) 1994, 1996, 1997, 1998 Free Software Foundation, Inc.
 *
 */

	.chip 68000

	.align 	2

	.globl	__mulsi3
__mulsi3:
	movew	%sp@(4), %d0	/* x0 -> %d0 */
	muluw	%sp@(10), %d0	/* x0*y1 */
	movew	%sp@(6), %d1	/* x1 -> %d1 */
	muluw	%sp@(8), %d1	/* x1*y0 */
	addl	%d1, %d0
	swap	%d0
	clrw	%d0
	movew	%sp@(6), %d1	/* x1 -> %d1 */
	muluw	%sp@(10), %d1	/* x1*y1 */
	addl	%d1, %d0
	rts

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

	.globl	__umodsi3
__umodsi3:
	movel	%sp@(8), %d1	/* %d1 = divisor */
	movel	%sp@(4), %d0	/* %d0 = dividend */
	movel	%d1, %sp@-
	movel	%d0, %sp@-
	bsr	__udivsi3
	addql	#8, %sp
	movel	%sp@(8), %d1	/* %d1 = divisor */
	movel	%d1, %sp@-
	movel	%d0, %sp@-
	bsr	__mulsi3	/* %d0 = (a/b)*b */
	addql	#8, %sp
	movel	%sp@(4), %d1	/* %d1 = dividend */
	subl	%d0, %d1		/* %d1 = a - (a/b)*b */
	movel	%d1, %d0
	rts

	.globl	__modsi3
__modsi3:
	movel	%sp@(8), %d1	/* %d1 = divisor */
	movel	%sp@(4), %d0	/* %d0 = dividend */
	movel	%d1, %sp@-
	movel	%d0, %sp@-
	bsr	__divsi3
	addql	#8, %sp
	movel	%sp@(8), %d1	/* %d1 = divisor */
	movel	%d1, %sp@-
	movel	%d0, %sp@-
	bsr	__mulsi3	/* %d0 = (a/b)*b */
	addql	#8, %sp
	movel	%sp@(4), %d1	/* %d1 = dividend */
	subl	%d0, %d1		/* %d1 = a - (a/b)*b */
	movel	%d1, %d0
	rts

/*	#include <ConditionalMacros.h>
 *	#include <Quickdraw.h>
 */

	.globl glue_display_properties
	.type	 glue_display_properties,@function

/*
 * void
 * glue_display_properties(unsigned long *base, unsigned long *row_bytes,
 *			   unsigned long *width, unsigned long *height,
 *			   unsigned long *depth, unsigned long* video)
 */

glue_display_properties:
	/*
         * GDHandle        hdl;
	 * volatile PixMapPtr       pm;
	 */

	link.w %a6,#-4

	movm.l #0x3c,-(%sp)
	move.l 28(%a6),%a3		/* unsigned long *video */
	move.l 12(%a6),%a5		/* unsigned long *row_bytes */
	move.l 24(%a6),%a4		/* unsigned long *depth */

	/* hdl = LMGetMainDevice(); */

	move.l     0x08A4,%a0

	/* if (hdl == NULL || (**hdl).gdPMap == NULL)
	 *	return;
	 */

	cmpa.l	#0, %a0
	jbeq .exit
	move.l (%a0),%a0
	tst.l 22(%a0)
	jbeq .exit

	/* pm = *(**hdl).gdPMap; */

	move.l 22(%a0),%a0

	/* if (pm->baseAddr == NULL)
	 *	return;
	 */

	move.l (%a0),-4(%a6)
	move.l -4(%a6),%a0
	tst.l (%a0)
	jbeq .exit

	/* *video = (unsigned char *)pm->baseAddr; */

	move.l -4(%a6),%a0
	move.l (%a0),(%a3)

	/* *row_bytes = pm->rowBytes & 0x3fff; */

	move.l -4(%a6),%a0
	move.w 4(%a0),%d0
	and.l #16383,%d0
	move.l %d0,(%a5)

	/* *width = pm->bounds.right - pm->bounds.left; */

	move.l -4(%a6),%a0
	move.w 12(%a0),%a2
	move.l -4(%a6),%a1
	move.l 16(%a6),%a0
	sub.w 8(%a1),%a2
	move.l %a2,(%a0)

	/* *height = pm->bounds.bottom - pm->bounds.top; */

	move.l -4(%a6),%a0
	move.w 10(%a0),%a2
	move.l -4(%a6),%a1
	move.l 20(%a6),%a0
	sub.w 6(%a1),%a2
	move.l %a2,(%a0)

	/* *depth = pm->pixelSize; */

	move.l -4(%a6),%a0
	move.w 32(%a0),%a0
	move.l %a0,(%a4)

	/* if (*depth == 15) *depth = 16; */

	moveq.l #15,%d0
	cmp.l %a0,%d0
	jbne .not_15
	moveq.l #16,%d0
	move.l %d0,(%a4)
.not_15:

	/* (*base) = (*video) */

	move.l	(%a3),%d0
	move.l 8(%a6),%a3		/* unsigned long *base */
	move.l	%d0,(%a3)

	/* (*base) = (*video) + pm->bounds.top * *row_bytes; */

	move.l -4(%a6),%a0
	move.w 6(%a0),%d0

	/* add 68000 support,
	 *
	 * row_bytes can be stored on a short
	 *
	 */

	move.l	(%a5), %d1
	muls.w %d1, %d0

	add.l %d0,(%a3)

	/* (*base) += pm->bounds.left * ((*depth) >> 3); */

	move.l -4(%a6),%a0
	move.w 8(%a0),%d1
	move.l (%a4),%d0
	lsr.l #3,%d0

	/* add 68000 support,
	 *
	 * NOTE: depth can be stored on a short
	 *
	 */

	muls.w	%d0, %d1

	add.l %d1,(%a3)

.exit:
	movm.l (%sp)+,#0x3c00
	unlk %a6
	rts

	/* OSErr Gestalt(OSType selector, long * response) */

	.global Gestalt
Gestalt:
	link	%fp, #0
	move.l	8(%fp), %d0
	dc.w	0xA1AD			/* Gestalt */
	move.l	12(%fp), %a1
	move.l	%a0, %a1@
	unlk	%fp
	rts

	/* void ReadLocation(MachineLocation * loc) */

	.global ReadLocation
ReadLocation:
	link	%fp, #0
	move.l	8(%fp), %a0
	move.l	#0x000C00E4, %d0 /* ReadLocation */
	dc.w	0xA051		/* ReadXPRam */
	unlk	%fp
	rts

	/* void* NewPtr(unsigned long byteCount) */

	.global NewPtr
NewPtr:
	link	%fp, #0
	move.l	8(%fp), %d0
	dc.w	0xA11E		/* NewPtr */
	move.l	%a0, %d0
	unlk	%fp
	rts

	/* void DisposePtr(void* ptr) */

	.global DisposePtr
DisposePtr:
	link	%fp, #0
	move.l	8(%fp), %a0
	dc.w	0xA01F		/* DisposePtr */
	unlk	%fp
	rts

	/* OSErr PBReadSync(ParamBlockRec_t* paramBlock) */

	.global PBReadSync
PBReadSync:
	link	%fp, #0
	move.l	8(%fp), %a0
	dc.w	0xA002		/* PBReadSync */
	unlk	%fp
	rts

	/* OSErr PBWriteSync(ParamBlockRec_t* paramBlock) */

	.global PBWriteSync
PBWriteSync:
	link	%fp, #0
	move.l	8(%fp), %a0
	dc.w	0xA003		/* PBWriteSync */
	unlk	%fp
	rts

	/* OSErr PBOpenSync(ParmBlkPtr paramBlock) */

	.global PBOpenSync
PBOpenSync:
	link	%fp, #0
	move.l	8(%fp), %a0
	dc.w	0xA000		/* PBOpenSync */
	unlk	%fp
	rts

	/* OSErr PBCloseSync(ParmBlkPtr paramBlock) */

	.global PBCloseSync
PBCloseSync:
	link	%fp, #0
	move.l	8(%fp), %a0
	dc.w	0xA001		/* PBCloseSync */
	unlk	%fp
	rts

	/* OSErr PBControlSync(ParmBlkPtr paramBlock) */

	.global PBControlSync
PBControlSync:
	link	%fp, #0
	move.l	8(%fp), %a0
	dc.w	0xA004		/* PBControlSync */
	unlk	%fp
	rts

	/* void SysError(short errorCode) */

	.global SysError
SysError:
	link	%fp, #0
	move.l	8(%fp), %d0
	dc.w	0xA9C9		/* SysError */
	unlk	%fp
	rts

#if defined(SCSI_SUPPORT)

	/* SCSI support */

.equ	_SCSIReset,	0x0000
.equ	_SCSIGet,	0x0001
.equ	_SCSISelect,	0x0002
.equ	_SCSICmd,	0x0003
.equ	_SCSIComplete,	0x0004
.equ	_SCSIRead,	0x0005
.equ	_SCSIWrite,	0x0006

.macro	SCSIDispatch selector
	move.w	#\selector, -(%sp)
	dc.w	0xA815			/* _SCSIDispatch */
	move.w	(%sp)+, %d0
	ext.l	%d0
.endm

	/* OSErr SCSIReset(void) */

	.global SCSIReset
SCSIReset:
	link	%fp, #0
	clr.w	-(%sp)
	SCSIDispatch(_SCSIReset)
	unlk	%fp
	rts

	/* OSErr SCSIGet(void) */

	.global SCSIGet
SCSIGet:
	link	%fp, #0
	clr.w	-(%sp)
	SCSIDispatch(_SCSIGet)
	unlk	%fp
	rts

	/* OSErr SCSISelect(short targetID) */

	.global SCSISelect
SCSISelect:
	link	%fp, #0
	move.l	8(%fp), %d0	/* targetID */
	clr.w	-(%sp)
	move.w	%d0, -(%sp)
	SCSIDispatch(_SCSISelect)
	unlk	%fp
	rts

	/* OSErr SCSICmd(void* buffer, short count) */

	.global SCSICmd
SCSICmd:
	link	%fp, #0
	move.l	8(%fp), %d0	/* buffer */
	move.l	12(%fp), %d1	/* count */
	clr.w	-(%sp)
	move.l	%d0, -(%sp)	/* buffer */
	move.w	%d1, -(%sp)	/* count */
	SCSIDispatch(_SCSICmd)
	unlk	%fp
	rts

	/* OSErr SCSIRead(void *tibPtr) */

	.global SCSIRead
SCSIRead:
	link	%fp, #0
	move.l	8(%fp), %d0	/* tibPtr */
	clr.w	-(%sp)
	move.l	%d0, -(%sp)
	SCSIDispatch(_SCSIRead)
	unlk	%fp
	rts

	/* OSErr SCSIComplete(short *stat, short *message, unsigned long wait)*/

	.global SCSIComplete
SCSIComplete:
	link	%fp, #0
	move.l	%d0, -(%sp)
	move.l	8(%fp), %d0	/* stat */
	move.l	12(%fp), %d1	/* message */
	move.l	16(%fp), %d2	/* wait */
	clr.w	-(%sp)
	move.l	%d0, -(%sp)
	move.l	%d1, -(%sp)
	move.l	%d2, -(%sp)
	SCSIDispatch(_SCSIComplete)
	unlk	%fp
	rts

#endif /* SCSI_SUPPORT */
