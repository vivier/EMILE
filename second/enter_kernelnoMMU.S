/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */
	.chip 68000

	.include "copymem.i"

	.align 4

	.global enter_kernelnoMMU
enter_kernelnoMMU:
	link.w  %fp,#0

	/* get physical start address of kernel */

	move.l	8(%fp), %a0

	/* size of image to relocate */

	move.l	12(%fp), %d0

	/* destination addresse */

	move.l	16(%fp), %a2

	/* copy kernel to destination address */

	move.l	%a2, %a1
	copymem	%a0, %a2, %d0

	/* jump into kernel */

	jmp	%a1@

	/* never comes here... I hope */

	unlk    %fp
	rts
	.global end_enter_kernelnoMMU
end_enter_kernelnoMMU:

	.global noMMU_disable_cache
noMMU_disable_cache:
	rts
