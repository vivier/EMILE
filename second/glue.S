/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

/*	#include <ConditionalMacros.h>
 *	#include <Quickdraw.h>
 */

	.align 	2
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

	tst.l %a0
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
	ext.l %d0
	muls.l (%a5),%d0
	add.l %d0,(%a3)

	/* (*base) += pm->bounds.left * ((*depth) >> 3); */

	move.l -4(%a6),%a0
	move.w 8(%a0),%d1
	ext.l %d1
	move.l (%a4),%d0
	lsr.l #3,%d0
	muls.l %d0,%d1
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

	/# void* NewPtr(unsigned long byteCount) */

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
