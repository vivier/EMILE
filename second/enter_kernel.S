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

	/* disable interrupts */

	ori.w	#0x0700,%sr

	/* disable MMU */

	lea	-8(%fp), %a1
	pmove	%tc, %a1@
	bclr	#7, %a1@
	pmove	%a1@, %tc

	clr.l	%a1@
	pmove	%a1@, %tt0
	pmove	%a1@, %tt1

	/* copy kernel to 0x1000 */

	move.l	%a0, %a1
	move.l	#0x1000, %a2
	move.l	%d0, %d1
	
copy_loop:
	move.b	(%a1)+, (%a2)+
	subq.l	#1, %d1
	bge.s	copy_loop

	/* disable and flush caches */

	lea	0x0808, %a1
	movec	%a1, %cacr
	
	/* jump into kernel */

	move.l	#0x1000, %a0
	jmp	%a0@

	/* never comes here... I hope */

	unlk    %fp
	rts
