/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

.equ	cmdline_length, 256		/* see CL_SIZE in bootinfo.c */

	.global _start
_start:
	jmp	setup(%pc)

	.word	cmdline_length		/* size of command line buffer */

	.global _command_line
_command_line:
	.skip	cmdline_length, 0

	.align	4
setup:
	/* identify system */

	bsr	arch_init

	/* initialize console, so we can debug ;-) */

	bsr	console_init

	/* initialize memory, so we can work */
	
	bsr	memory_init

	/* retrieve machine info */

	bsr	bootinfo_init;

	/* allocate stack, so we can jump */

	pea	0x2000
	bsr	malloc
	add.l	#0x2000 - 16, %d0
	move.l	%d0, %sp

	/* begin to work */

	bsr	main

	/* We guess to never come here */
loop:
	bra	loop
