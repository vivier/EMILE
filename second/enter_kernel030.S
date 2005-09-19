/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.chip 68030

	.include "copymem.i"

	.align 4

	.global enter_kernel030
enter_kernel030:
	link.w  %fp,#-8

	/* get physical start address of kernel */

	move.l	8(%fp), %a0

	/* size of image to relocate */

	move.l	12(%fp), %d0

	/* destination addresse */

	move.l	16(%fp), %a2

	/* bootenv address (EXPERIMENTAL) */

	move.l	%a2, %a1
	add.l	%d0, %a1
	sub.l	#4096, %a1
	move.l	#0, %d4

	/* entry point */

	move.l	20(%fp), %a3

	/* disable MMU */

	lea	-8(%fp), %a4
	pmove	%tc, %a4@
	bclr	#7, %a4@
	pmove	%a4@, %tc

	clr.l	%a4@
	pmove	%a4@, %tt0
	pmove	%a4@, %tt1

	/* copy kernel to destination address */

	copymem	%a0, %a2, %d0

	/* jump into kernel */

	jmp	%a3@

	/* never comes here... I hope */

	unlk    %fp
	rts
	.global end_enter_kernel030
end_enter_kernel030:
