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
	/* relocate C code, need to be compile with -fpic */

	bsr	relocate

	/* identify system */

	bsr	arch_init@PLTPC

	/* initialize console, so we can debug ;-) */

	bsr	console_init@PLTPC

	/* initialize memory, so we can work */
	
	bsr	memory_init@PLTPC

	/* retrieve machine info */

	bsr	bootinfo_init@PLTPC

	/* allocate stack, so we can jump */

	pea	0x2000
	bsr	malloc@PLTPC
	add.l	#0x2000 - 16, %d0
	move.l	%d0, %sp

	/* begin to work */

	bsr	main@PLTPC

	/* We guess to never come here */
loop:
	bra	loop

relocate:
	/* real address of GOT */

	lea	(%pc, _GLOBAL_OFFSET_TABLE_@GOTPC), %a0

	/* linked address of GOT */

	move.l	#_GLOBAL_OFFSET_TABLE_, %d0

	/* offset to add to GOT items */

	move.l	%a0,%d2
	sub.l	%d0,%d2
	beq	exit_relocate

	move.l	#_GOT_SIZE, %d0
	beq	exit_relocate

got_loop:
	move.l	(%a0), %d1
	add.l	%d2, %d1
	move.l	%d1, (%a0)+
	subq.l	#4, %d0
	bgt	got_loop

exit_relocate:
	rts
