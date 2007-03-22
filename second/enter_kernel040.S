/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

	.cpu 68040

	.include "copymem.i"

	.align 4

	.global enter_kernel040
enter_kernel040:
	link.w  %fp,#0

	/* get physical start address of kernel */

	move.l	8(%fp), %a0

	/* size of image to relocate */

	move.l	12(%fp), %d0

	/* destination address */

	move.l	16(%fp), %a2

	/* bootenv address (EXPERIMENTAL) */

	move.l  %a2, %a1
	add.l   %d0, %a1
	sub.l   #4096, %a1
	move.l	#0x80000000, %d4

	/* entry point */

	move.l	20(%fp), %a3

	/* disable MMU */

	moveq.l	#0, %d1
	movec	%d1, %tc
	movec	%d1, %itt0
	movec	%d1, %dtt0
	movec	%d1, %itt1
	movec	%d1, %dtt1

	/* copy kernel to destination address */

	copymem	%a0, %a2, %d0

	/* jump into kernel */

	jmp	%a3@

	/* never comes here... I hope */

	unlk    %fp
	rts
	.global end_enter_kernel040
end_enter_kernel040:
