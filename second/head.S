/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

.macro NewPtr
        .short 0xA11E
.endm

.equ	cmdline_length, 256		/* see CL_SIZE in bootinfo.c */

	.global _start
_start:
	bra	setup

/***************************************************************************
 *
 * second level arguments 
 *
 ***************************************************************************/

	/* Header Signature */

	.dc.b	'E','M','0','1'
	
	.global _kernel_image_offset
_kernel_image_offset:
	.long	0

	.global _kernel_image_size
_kernel_image_size:
	.long	0

	.global _kernel_size
_kernel_size:
	.long	0

	.global _ramdisk_offset
_ramdisk_offset:
	.long	0

	.global _ramdisk_size
_ramdisk_size:
	.long	0

	.global _command_line
_command_line:
	.skip	cmdline_length, 0

	.align	4
setup:
	/* relocate C code, need to be compiled with -fpic */

	bsr	relocate

	/* initialize console, so we can debug ;-) */

	bsr	console_init@PLTPC

	/* begin to work */

	bsr	main@PLTPC

	/* We guess to never come here */
loop:
	bra	loop

relocate:
	/* Allocate BSS section */

	move.l	#__bss_size, %d0	/* size */
	NewPtr					/* result in %a0 */
	move.l	%a0,%d3
	move.l	#__bss_start, %d0
	sub.l	%d0,%d3			/* %d3 is the offset to add for BSS */

	/* real address of GOT */

	lea	(%pc, _GLOBAL_OFFSET_TABLE_@GOTPC), %a0

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
