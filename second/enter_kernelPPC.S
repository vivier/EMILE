/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 * 	    from BootX, (c) Benjamin Herrenschmidt
 *
 */

	.chip 68020

	.include "copymem.i"

.equ	PC,  0x00FC
.equ	GPR, 0x0100
.equ	FPR, 0x0200

#define pc	PC(%sp)
#define gpr(a)	GPR + 8 * a(%sp)
#define fpr(a)	FPR + 8 * a(%sp)

	.align 4

	.global enter_kernelPPC
enter_kernelPPC:
	link.w  %fp,#-8

	/* get physical start address of kernel */

	move.l	8(%fp), %a0

	/* size of image to relocate */

	move.l	12(%fp), %d0

	/* destination addresse */

	move.l	16(%fp), %a2

	/* copy kernel to destination address */

	move.l	%a2, %a1
	
	copymem	%a0, %a2, %d0

	/* prepare stack */

	move.l	%sp, %d0
	andi.l	#0xFFFFFC00, %d0
	move.l	%d0, %sp
	move.l	#0x00BF, %d0
loop:
	clr.l	-(%sp)
	dbra	%d0, loop

	/* Initialize PPC registers */

	move.l	#0x426f6f58, gpr(3)	/* 'BooX' */
	move.l	#bootx_infos, gpr(4)
	move.l	#0, gpr(5)

	/* set up program counter */

	move.l	%a1, pc

	/* Switch to PPC */

	move.l	#0x47617279, %a0 /* 'Gary' */
	move.l	#0x05051956, %a1
	move.l	#0x0000C000, %d0
	moveq	#0, %d2
	reset

	move.l  %sp,-(%sp)
wait:
	dc.w	0xFE03
	beq	wait
	rts
	.global end_enter_kernelPPC
end_enter_kernelPPC:
