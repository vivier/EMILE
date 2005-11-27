/*
 *
 * (c) 2004, 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.chip 68000

.macro SysError
	.short 0xA9C9
.endm

.macro NewPtrClear
        .short 0xA31E
.endm

.equ	paramstring_length, 1024

/***************************************************************************
 *
 * second level arguments 
 *
 ***************************************************************************/

_start:			bra	setup
_signature:		.dc.b	'E','M','0','6'

/* EM06 */

_conf_size:		.dc.w	paramstring_length
_configuration:		.skip	paramstring_length, 0

	.align	4
setup:
	/* relocate C code, need to be compiled with -fpic */

	bsr	relocate

	/* initialize console, so we can debug ;-) */

	lea	_start(%pc), %a0
	move.l	%a0, -(%sp)
	bsr	console_init@PLTPC

	/* begin to work */

	lea	_start(%pc), %a0
	move.l	%a0, -(%sp)
	bsr	start@PLTPC

	/* We guess to never come here */
loop:
	bra	loop

relocate:
	/* Allocate BSS section */

	move.l	#__bss_size, %d0	/* size */
	/* Alloc and clear memory (needed by GCC 3) */
	NewPtrClear			/* result in %a0 */
	move.l	%a0, %d0
	bne	malloc_ok
	SysError
malloc_ok:
	move.l	%a0,%d3
	move.l	#__bss_start, %d0
	sub.l	%d0,%d3			/* %d3 is the offset to add for BSS */

	/* real address of GOT */

	lea	_GLOBAL_OFFSET_TABLE_-.-8,%a0
	lea	(%pc, %a0), %a0

	/* linked address of GOT */

	move.l	#_GLOBAL_OFFSET_TABLE_, %d0

	/* %d2 is the offset to add for all other sections */

	move.l	%a0,%d2
	sub.l	%d0,%d2

	/* relocate BSS section */

	move.l	#__got_size, %d0
	beq	exit_relocate

got_loop:
	move.l	(%a0), %d1

	cmp.l	#__bss_start.l, %d1
	blt	other_section
	cmp.l	#_end.l, %d1
	bgt	other_section

	/* symbol in section BSS */

	add.l	%d3, %d1
	bra	got_store

other_section:
	add.l	%d2, %d1

got_store:
	move.l	%d1, (%a0)+

	subq.l	#4, %d0
	bgt	got_loop

exit_relocate:
	rts
