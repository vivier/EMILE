/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.global enter_kernel
enter_kernel:
	link.w  %fp,#-8

	/* get physical start address of kernel */

	move.l	8(%fp), %a0

	/* size of image to relocate */

	move.l	12(%fp), %d0

	/* disable MMU */

	lea	-8(%fp), %a1
	pmove	%tc, %a1@
	bclr	#7, %a1@
	pmove	%a1@, %tc

	clr.l	%a1@
	pmove	%a1@, %tt0
	pmove	%a1@, %tt1

	/* copy kernel to 0x1000 */

	move.l	#0x1000, %a2
	move.l	%a2, %a1
	
copy_loop:
	move.b	(%a0)+, (%a2)+
	subq.l	#1, %d0
	bge.s	copy_loop

	/* jump into kernel */

	jmp	%a1@

	/* never comes here... I hope */

	unlk    %fp
	rts
	.global end_enter_kernel
end_enter_kernel:
